#include "HelloWorldScene.h"

USING_NS_CC;

#define COIN_WIDTH 212 //coin 图片宽度
#define COIN_GAP 100    //间隔
#define COIN_COUNT 11   //个数

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    Size visibleSize = Director::getInstance() -> getVisibleSize();
    
    //XKPageView::create(Size size,XKPageViewDelegate *delegate)
    pageView = XKPageView::create(Size(visibleSize.width, COIN_WIDTH), this);
    pageView -> setDirection(ScrollView::Direction::HORIZONTAL);
    //container 定位在屏幕中间
    pageView -> setPosition(Point((visibleSize.width - COIN_WIDTH) * 0.5, (visibleSize.height - COIN_WIDTH) * 0.5));
    
    addPages();
    //设置裁切为false, 这样layer 溢出pageView的Size还能显示
    pageView -> setClippingToBounds(false);
    this -> addChild(pageView);
    
    //设置pageView 自动调整时的callback,可以在这里写你要的功能
    pageView -> adjustCallback = [](XKPageView *pageView){
        
        FiniteTimeAction *fadeIn = FadeTo::create(0.15, 255);
        FiniteTimeAction *fadeOut = FadeTo::create(0.15, 255 * 0.3);
        FiniteTimeAction *scaleToBig = ScaleTo::create(0.15, 1.5f);
        FiniteTimeAction *scaleToSmall = ScaleTo::create(0.15, 1.0f);
        
        Spawn *spawnIn = Spawn::createWithTwoActions(fadeIn, scaleToBig);
        Spawn *spawnOut = Spawn::createWithTwoActions(fadeOut, scaleToSmall);
        
        int current = pageView -> getCurrentIndex();
        auto sprite = (Sprite*) pageView -> getPageAtIndex(current);
        sprite -> runAction(spawnIn);
        
        sprite = (Sprite *)pageView -> getPageAtIndex(current - 1);
        if (sprite !=NULL){
            sprite -> runAction(spawnOut);
        }
        sprite = (Sprite *)pageView -> getPageAtIndex(current + 1);
        if (sprite !=NULL){
            sprite -> runAction(spawnOut -> clone());
        }
    };
    
    return true;
}

void HelloWorld::addPages()
{
    Size coinSize = Sprite::create("coin.png") -> getContentSize();
    //11个layer 加到layer 上
    for (int i = 0; i < COIN_COUNT; i++) {
        auto sprite = Sprite::create("coin.png");
        sprite -> setPosition(coinSize.width * 0.5, coinSize.height * 0.5);
        std::string str = StringUtils::format("%d", i);
        Label *label = Label::createWithSystemFont(str, "Arial", 60);
        label -> setTextColor(Color4B(0,0,0,255));
        Size size = sprite -> getContentSize();
        label -> setPosition(size.width * 0.5, size.height * 0.5);
        sprite -> addChild(label);
        //注意这里使用的时XKPageView 的addPage, 不是addChild
        pageView -> addPage(sprite);
    }
}

Size HelloWorld::sizeForPerPage()
{
    //Delegate 的东西，返回每个Page 的Size
    return Size(COIN_WIDTH + COIN_GAP, COIN_WIDTH);
}

void HelloWorld::pageViewDidScroll(XKPageView *pageView)
{
    //监听滚动时间，可以再这里写滚动时候要添加的代码,比如缩放~
    Size visibleSize = Director::getInstance() -> getVisibleSize();
    float midX = visibleSize.width / 2;
    float offsetX = pageView -> getContentOffset().x;
    float tmp = COIN_WIDTH / 2.0f;
    float scale = 1.5;
    scale = scale - 1;

    
    for (int i = 0; i < COIN_COUNT; i++) {
        auto sprite = (Sprite *) pageView -> getPageAtIndex(i);
        float positionX = sprite -> getPositionX();
        //转换成相对屏幕坐标
        float endX = positionX + offsetX + midX - tmp;
        if (0 < endX && endX <= midX) {
            float x = endX / midX * scale + 1;
            sprite -> setScale(x);
            x = (endX / midX * 0.7 + 0.3) * 255;
            sprite -> setOpacity(x);
        }else if(endX > midX && endX < visibleSize.width){
            float tmp2 = endX - midX;
            tmp2 = midX - tmp2;
            float x = tmp2 / midX * scale + 1;
            sprite -> setScale(x);
            
            x = (tmp2 / midX * 0.7 + 0.3) * 255;
            sprite -> setOpacity(x);
        }else {
            sprite -> setScale(1.0f);
            sprite -> setOpacity(255 * 0.3);
        }
    }
}

void HelloWorld::onEnter()
{
    Layer::onEnter();
    pageView -> adjustCallback(pageView);
}
