#include "Mouse.hpp"

std::optional<Mouse::RawDelta> Mouse::read_raw_delta() noexcept
{
    if(rawDeltaBuffer.empty()) return std::nullopt;

    const RawDelta d = rawDeltaBuffer.front();
    rawDeltaBuffer.pop();
    return d;
}

std::pair<int,int> Mouse::get_pos() const noexcept { return {x, y}; }
int Mouse::get_x() const noexcept { return x; }
int Mouse::get_y() const noexcept { return y; }


int Mouse::get_wheel_delta_carry() const noexcept { return wheelDeltaCarry; }

bool Mouse::is_in_window() const noexcept { return isInWindow; }
bool Mouse::left_is_pressed() const noexcept { return leftIsPressed; }
bool Mouse::right_is_pressed() const noexcept { return rightIsPressed; }

void Mouse::enable_raw() noexcept { rawEnabled = true; }
void Mouse::disable_raw() noexcept { rawEnabled = false; }
bool Mouse::raw_enabled() const noexcept { return rawEnabled; }

std::optional<Mouse::Event> Mouse::read() noexcept
{
    if(!buffer.empty()) 
    {
        Event e = buffer.front();
        buffer.pop();
        return e;
    }
    return std::nullopt;
}

bool Mouse::is_empty() const noexcept { return buffer.empty(); }
void Mouse::clear() noexcept { while (!buffer.empty()) buffer.pop(); }



void Mouse::on_mouse_move(int _x, int _y) noexcept
{
    x = _x;
    y = _y;
    buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
    trim_buffer();
}
void Mouse::on_mouse_enter() noexcept
{
    isInWindow = true;
    buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
    trim_buffer();
}
void Mouse::on_mouse_leave() noexcept
{
    isInWindow = false;
    buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
    trim_buffer();
}
void Mouse::on_left_press(int x, int y) noexcept
{
    leftIsPressed = true;
    buffer.push(Mouse::Event(Mouse::Event::Type::LeftPress, *this));
    trim_buffer();
}
void Mouse::on_right_press(int x, int y) noexcept
{
    rightIsPressed = true;
    buffer.push(Mouse::Event(Mouse::Event::Type::RightPress, *this));
    trim_buffer();
}
void Mouse::on_left_release(int x, int y) noexcept
{
    leftIsPressed = false;
    buffer.push(Mouse::Event(Mouse::Event::Type::LeftRelease, *this));
    trim_buffer();
}
void Mouse::on_right_release(int x, int y) noexcept
{
    rightIsPressed = false;
    buffer.push(Mouse::Event(Mouse::Event::Type::RightRelease, *this));
    trim_buffer();
}
void Mouse::on_wheel_up(int x, int y) noexcept
{
    buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
    trim_buffer();
}
void Mouse::on_wheel_down(int x, int y) noexcept
{
    buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
    trim_buffer();
}
void Mouse::on_wheel_delta(int x, int y, int delta) noexcept
{
    wheelDeltaCarry += delta;
    while(wheelDeltaCarry >= WHEEL_DELTA)
    {
        wheelDeltaCarry -= WHEEL_DELTA;
        on_wheel_up(x,y);
    }
    while(wheelDeltaCarry <= -WHEEL_DELTA)
    {
        wheelDeltaCarry += WHEEL_DELTA;
        on_wheel_down(x,y);
    }
}
void Mouse::on_raw_delta(int dx, int dy) noexcept
{
    if (!rawEnabled) return;
    rawDeltaBuffer.push({dx, dy});
    trim_raw_buffer();
}

void Mouse::trim_buffer() noexcept
{
    while(buffer.size() > bufferSize) buffer.pop();
}
void Mouse::trim_raw_buffer() noexcept
{
    while(rawDeltaBuffer.size() > bufferSize) rawDeltaBuffer.pop();
}