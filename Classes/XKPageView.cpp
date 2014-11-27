//
//  XKPageView.cpp
//  XKPageView
//
//  Created by Joueu on 14-11-26.
//
//

#include "XKPageView.h"

#define XKPAGEVIEW_TAG 10086

XKPageView *XKPageView::create(Size size,XKPageViewDelegate *delegate)
{
    XKPageView *page = new XKPageView();
    if (page && page -> init(size,delegate)) {
        page -> autorelease();
    }else {
        CC_SAFE_RELEASE(page);
    }
    return  page;
}

bool XKPageView::init(Size size,XKPageViewDelegate *delegate)
{
    if (!ScrollView::initWithViewSize(size)) {
        return false;
    }
    //必须有delegate，否则断掉
    CCASSERT(delegate, "delegate should not be NULL!");
    setDelegate(delegate);
    if (_delegate) {
        //获取page的大小
        pageSize = _delegate -> sizeForPerPage();
    }
    //init Data
    pageCount = 0;
    current_index = 0;
    
    this -> setTouchEnabled(false);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener -> onTouchBegan = [&](Touch *touch, Event *event){
        _dragging = false;
        this -> scheduleUpdate();
        if (_direction == ScrollView::Direction::HORIZONTAL) {
            current_offset = this -> getContentOffset().x;
        }else {
            current_offset = this -> getContentOffset().y;
        }
        return true;
    };
    listener -> onTouchMoved = [&](Touch *touch, Event *event){
        float start, end;
        if (_direction == ScrollView::Direction::HORIZONTAL) {
            start = touch -> getStartLocation().x;
            end = touch -> getLocation().x;
        }else {
            start = touch -> getStartLocation().y;
            end = touch -> getLocation().y;
        }
        float offset = end - start;
        // * 2的作用是调节滚动速度，需要调滑动速度的 可以改这个值
        if (_direction == ScrollView::Direction::HORIZONTAL)
            this -> setContentOffset(Vec2(current_offset + offset * 2, 0));
        else
            this -> setContentOffset(Vec2(0, current_offset + offset * 2));
    };
    listener -> onTouchEnded = [&](Touch *touch, Event *event){
        float start = current_offset, end;
        if (_direction == ScrollView::Direction::HORIZONTAL) {
            end = this -> getContentOffset().x;
        }else {
            end = this -> getContentOffset().y;
        }
        float offset = end - start;
        this -> adjust(offset);
        _dragging = true;
    };
    Director::getInstance() -> getEventDispatcher() -> addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void XKPageView::adjust(float offset)
{
    Vec2 vec;
    float xOrY;
    if (_direction == ScrollView::Direction::HORIZONTAL) {
        vec = Vec2(-( current_index * (pageSize.width)),0);
        xOrY = pageSize.width;
    }else {
        vec = Vec2(0, -( current_index * (pageSize.height)));
        xOrY = pageSize.height;
    }
    //小于50回到原来位置
    if  (abs(offset) < 50){
        this -> setContentOffsetInDuration(vec,0.15f);
        return;
    }
    
    int i = abs(offset / (xOrY)) + 1;
    if (offset < 0) {
        current_index += i;
    }else {
        current_index -= i;
    }
    
    if (current_index < 0) {
        current_index = 0;
    }else if(current_index > 10){
        current_index = 10;
    }
    
    if (_direction == ScrollView::Direction::HORIZONTAL) {
        vec = Vec2(-( current_index * (pageSize.width)),0);
    }else {
        vec = Vec2(0, -( current_index * (pageSize.height)));
    }
    this -> adjustCallback(this);
    this -> setContentOffsetInDuration(vec, 0.15f);
}

void XKPageView::setContentOffset(Vec2 offset)
{
    ScrollView::setContentOffset(offset);
    if (_delegate != nullptr)
    {
        _delegate -> pageViewDidScroll(this);
    }
}

void XKPageView::setContentOffsetInDuration(Vec2 offset, float dt)
{
    ScrollView::setContentOffsetInDuration(offset, dt);
    this->schedule(CC_SCHEDULE_SELECTOR(XKPageView::performedAnimatedScroll));
}

void XKPageView::performedAnimatedScroll(float dt)
{
    if (_dragging)
    {
        this->unschedule(CC_SCHEDULE_SELECTOR(XKPageView::performedAnimatedScroll));
        this -> unscheduleUpdate();
        return;
    }
    
    if (_delegate != nullptr)
    {
        _delegate -> pageViewDidScroll(this);
    }
}

void XKPageView::addPage(Node *node)
{
    if (_direction == ScrollView::Direction::HORIZONTAL) {
        node -> setPosition(Point(pageCount * pageSize.width + node -> getPositionX(),node -> getPositionY()));
        this -> setContentSize(Size((pageCount + 1) * pageSize.width,pageSize.height));
    }else {
        node -> setPosition(Point(node -> getPositionX(),pageCount * pageSize.height + node -> getPositionY()));
        this -> setContentSize(Size(pageSize.width,(pageCount + 1) *pageSize.height));
    }
    node -> setTag(pageCount + XKPAGEVIEW_TAG);
    _container -> addChild(node);
    pageCount ++;
    
}

Node *XKPageView::getPageAtIndex(int index)
{
    if (index < pageCount && index >= 0) {
        return _container -> getChildByTag(index + XKPAGEVIEW_TAG);
    }
    return  NULL;
}

int XKPageView::getCurrentIndex()
{
    return current_index;
}