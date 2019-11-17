CREATE VIEW eventsview as select sub.masterid as useid, cnt.eventid, cnt.channelid, cnt.source, GREATEST(cnt.updsp,sub.updsp,IFNULL(epi.updsp,0)) updsp, cnt.updflg, cnt.delflg, cnt.fileref, cnt.tableid, cnt.version, sub.title,
case
 when sub.shorttext is null then
  case when sub.genre is null then '' else concat(sub.genre,' (', sub.country, ' ',sub.year, ')') end
 else sub.shorttext
end shorttext,
cnt.starttime, cnt.duration, cnt.parentalrating, cnt.vps, sub.longdescription as description
from
 events cnt
 inner join events sub on (case when cnt.useid = 0 then cnt.masterid else cnt.useid end = sub.masterid)
 left outer join episodes epi on (sub.episode = epi.compname and sub.episodepart = epi.comppartname and sub.episodelang = epi.lang);
