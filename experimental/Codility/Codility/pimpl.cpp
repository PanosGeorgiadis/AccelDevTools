//widget.h
class widget
{
private:
    struct widget_impl;

    std::unique_ptr<widget_impl> _pImpl;
};

//widget.cpp
struct widget::widget_impl
{
//...implementation details
};
