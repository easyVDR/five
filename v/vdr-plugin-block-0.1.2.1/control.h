/**
 * based on control.h,v 1.2 2007/06/18 17:58:52 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_CONTROL_H
#define VDR_BLOCK_CONTROL_H

#include <vdr/player.h>

class cControlBlock : public cControl {
private:
	static bool           mRequested;
	
	const cChannel       *mChannel;
	const cEvent         *mPresent;
	const cEvent         *mFollowing;
	uint64_t              mStart;
	bool                  mSwitch;
	cSkinDisplayChannel  *mOsd;
	int direction;

protected:
	virtual void Show(void);
  virtual void Hide(void) {}

public:
  cControlBlock(const cChannel *Channel, const cEvent *Present, const cEvent *Following);
  ~cControlBlock();

  virtual eOSState ProcessKey(eKeys Key);

	static bool IsRequested(void) { bool r = mRequested; mRequested = false; return r; }
};

#endif // VDR_BLOCK_CONTROL_H
