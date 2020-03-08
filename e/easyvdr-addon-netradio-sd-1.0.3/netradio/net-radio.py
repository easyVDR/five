#!/usr/bin/env python3
#
# script fuer easyvdr geaendert
# mango 05.01.2012
# mango add new url 26.04.2013
# mango add new url 08.09.2015
# 03.03.2020 - new python3-script thx seahawk1986(vdr-portal)
import urllib.request
import urllib.parse
import sys
import re
import shutil
import string
import subprocess

from collections import defaultdict
from pathlib import Path
from typing import List, Dict, Any, Mapping

# ensure outputdir exists and is empty
outdir = Path("/media/easyvdr01/net-radio")
outdir.mkdir(parents=True, exist_ok=True)

for f in outdir.iterdir():
    if f.is_file():
        f.unlink()
    elif f.is_dir():
        shutil.rmtree(f)

# ensure tmpdir exists
tmpdir = Path("/tmp/vtn/")
tmpdir.mkdir(parents=True, exist_ok=True)

url = "http://www.vtuner.com/vtunerapp/vtuner4vf/asp/appupdate/updateflush.asp?Flush=No"


category_translations = {
    "Talk": "sprache",
    "Music": "musik",
    "Television": "fernsehen",
    "Webcam": "webcams",

}
region_translations = {
    "Africa": "afrika",
    "Asia": "asien",
    "Caribbean": "karibik",
    "Europe": "europa",
    "Middle East": "mitost",
    "North America": "nordamerika",
    "Oceania": "ozeanien",
    "Internet Only": "internet",
    "South America": "südamerika",
    "Central America": "zentralamerika",
}


def load_data() -> List[str]:
    """load stations from disk or get it from the server"""
    try:
        with open(tmpdir / "stations", "r") as f:
            lines = f.readlines()
    except IOError:
        print("get ", url)
        with urllib.request.urlopen(url) as response:
            data = response.read().decode("latin9")
        with open(tmpdir / "stations", "w") as f:
            f.write(data)
            lines = data.splitlines()
    finally:
        print("found stations")
        return lines


def parse_data(entries: List[str]) -> Mapping[str, Mapping[str, Any]]:
    coder = {}
    name = {}
    stream = {}
    location = {}
    music = {}
    channels_by_region = defaultdict(dict)
    channels_by_category = defaultdict(dict)
    fun_channels = dict()
    categories = {}
    locations = {}

    def parse_url(raw_url: str) -> str:
        def decode_string(e: str) -> str:
            """Zeichen in den URLs korrigieren (schein ein halbgares ROT-14 zu sein)"""
            intab = string.ascii_lowercase + string.ascii_uppercase + "!@#$%^&*()<>6"
            rotated_lowercase = string.ascii_lowercase[14:] + string.ascii_lowercase[:14]
            rotated_uppercase = string.ascii_uppercase[14:] + string.ascii_uppercase[:14]
            outtab = rotated_lowercase + rotated_uppercase + "0123456789:=&"
            trantab = str.maketrans(intab, outtab)
            return e.translate(trantab)

        url = decode_string(raw_url)
        if url.startswith("m"):  # cleanup mhttp addresses
            url = url[1:]
        try:
            o = urllib.parse.urlparse(url)
            params = urllib.parse.parse_qs(o.query)
            if (links := params.get("link")):
                s, *_ = links
            else:
                s = urllib.parse.urlunsplit((*o[:3], "", ""))
            return s
        except Exception as err:
            print("could not parse url {url}: {err}", file=sys.stderr)

    print("parsing...")

    fileName = None
    start_re = re.compile(r"(\S+) - start:")
    stop_re = re.compile(r"(\S+) - end:")

    line_gen = iter(entries)
    for line in line_gen:
        line = line.strip()
        if not line or line.startswith("date::") or line.startswith("file::"):
            continue
        if (match := start_re.match(line)) :
            fileName, *_ = match.groups()
            print("parsing", fileName)
            file_stem = Path(fileName).stem
            # Next line(s) contain file content
            while not (match := stop_re.match((line := next(line_gen).strip()))):
                if not line:
                    continue
                fields = line.split("\t")
                if fileName == "station.vtn":
                    try:
                        (
                            key,
                            name_value,
                            location_value,
                            music_value,
                            coder_value,
                            domain,
                            url,
                            *_,
                        ) = fields
                    except ValueError:
                        print("invalid line in station.vtn", file=sys.stderr)
                        continue
                    if not coder_value.startswith("MP3"):
                        continue  # skip non-mp3 streams
                    name[key] = name_value
                    location[key] = location_value
                    music[key] = music_value
                    coder[key] = coder_value
                    stream[key] = parse_url(url)
                elif fileName == "cat.vtn":
                    try:
                        cat_n, category, short, abbrev = fields
                    except ValueError:
                        print(f"invalid line in {fileName}: {line}", file=sys.stderr)
                    else:
                        categories[short] = category
                elif fileName == "catloc.vtn":
                    try:
                        key, name_value, short, r_key = fields
                    except ValueError:
                        print(f"invalid line in {fileName}: {line}", file=sys.stderr)
                    else:
                        locations[short] = name_value
                elif fileName == "fun.vtn":
                    sub_category, stations = fields
                    fun_channels[f"fun/{sub_category}"] = stations.split(" ")
                    print("processing fun.vtn", sub_category, stations.split(' '))
                elif file_stem in locations:
                    try:
                        n, m, region_name, zero, ids = fields
                    except ValueError:
                        print(f"invalid line in {fileName}: {line}", file=sys.stderr)
                    else:
                        channels_by_region[locations[file_stem]][
                            region_name
                        ] = ids.split(" ")
                elif file_stem in categories:
                    n, m, subcat_name, zero, ids = fields
                    channels_by_category[
                            f"{category_translations.get(categories[file_stem], categories[file_stem])}/{subcat_name}"] = ids.split(" ")
    return {
        "coder": coder,
        "name": name,
        "stream": stream,
        "location": location,
        "music": music,
        "channels_by_region": channels_by_region,
        "channels_by_category": channels_by_category,
        "fun_channels": fun_channels,
    }


def createpls(basedir: Path, data: Mapping[str, Any]):
    coder: dict = data["coder"]
    name: dict = data["name"]
    stream: dict = data["stream"]
    location: dict = data["location"]
    music: dict = data["music"]
    channels_by_category: defaultdict = data["channels_by_category"]
    channels_by_region: defaultdict = data["channels_by_region"]
    fun_channels: dict = data["fun_channels"]

    def write_m3u(basedir: Path, channel_id: str) -> None:
        if channel_id in name and channel_id in music:
            fname = f"{name[channel_id]} ({music[channel_id]}).m3u".replace("/", "_").replace('*', '_')
            target_file = basedir / fname
            target_file.write_text(stream[channel_id])
            print(f"written {target_file}")
        else:
            print(f"unknown channel_id {channel_id} {name[channel_id] if channel_id in name else ''}", file=sys.stderr)

    for region, country_data in channels_by_region.items():
        for country, ids in country_data.items():
            target_dir = basedir / "laender" / region_translations.get(region, region) / country
            target_dir.mkdir(parents=True, exist_ok=True)
            for channel_id in ids:
                write_m3u(basedir=target_dir, channel_id=channel_id)

    for category, ids in fun_channels.items():
        target_dir = basedir / category
        target_dir.mkdir(parents=True, exist_ok=True)
        for channel_id in ids:
            write_m3u(basedir=target_dir, channel_id=channel_id)

    for category, ids in channels_by_category.items():
        target_dir = basedir / category
        #try:
        #    target_dir /= music[channel_id]
        #except KeyError:
        #    pass
        target_dir.mkdir(parents=True, exist_ok=True)
        for channel_id in ids:
            write_m3u(basedir=target_dir, channel_id=channel_id)


if __name__ == "__main__":
    entries = load_data()
    data = parse_data(entries)
    createpls(outdir, data)

    # set permissions for the created files and folders
    subprocess.run(["chown", "-R", "vdr:vdr", outdir])
    # svdrpsend message to VDR
    subprocess.run(
        ["svdrpsend", "mesg", "Parsen und erstellen von Playlisten beendet!"]
    )
    
