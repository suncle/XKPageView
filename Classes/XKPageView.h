//
//  XKPageView.h
//  XKPageView
//
//  Created by Joueu on 14-11-26.
//
//

#ifndef __XKPageView__XKPageView__
#define __XKPageView__XKPageView__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class XKPageView;
class XKPageViewDelegate
{
public:
    
    virtual ~XKPageViewDelegate(){};
    XKPageViewDelegate(){};
    //获取Page大小
    virtual Size sizeForPerPage() = 0;
    //滚动回调
    virtual void pageViewDidScroll(XKPageView *pageView){};
};

class XKPageView:public ScrollView
{
public:
    static XKPageView *create(Size size,XKPageViewDelegate *delegate);
    virtual bool init(Size size,XKPageViewDelegate *delegate);
public:
    void setPageSize(Size size);
    virtual void setContentOffsetInDuration(Vec2 offset, float dt);
    virtual void setContentOffset(Vec2 offset);
private:
    void performedAnimatedScroll(float dt);
    int current_index;
    float current_offset;
    //调整offset 的函数
    void adjust(float offset);
    Size pageSize;
    CC_SYNTHESIZE(XKPageViewDelegate *, _delegate, Delegate);
public:
    int pageCount;
    void addPage(Node *node);
    Node *getPageAtIndex(int index);
    int getCurrentIndex();
    //滚动后调整回调
    std::function<void(XKPageView *)> adjustCallback;
};

#endif /* defined(__XKPageView__XKPageView__) */
