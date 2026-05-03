#pragma once

#include <optional>
#include <queue>

class Mouse
{
public:

    struct RawDelta { int x; int y; };

    class Event
    {
        public:
            enum class Type
            {
                LeftPress,
                LeftRelease,

                RightPress,
                RightRelease,

                WheelUp,
                WheelDown,

                Move,
                Enter,
                Leave,

                Invalid
            };

        private:
            Type type;
            bool leftIsPressed;
            bool rightIsPressed;
            int x;
            int y;

        public:
            Event() noexcept : type(Type::Invalid), 
                               leftIsPressed(false), 
                               rightIsPressed(false), 
                               x(0), y(0) {}

            Event(Type t, const Mouse& parent) noexcept : type(t), 
                                                          leftIsPressed(parent.leftIsPressed),
                                                          rightIsPressed(parent.rightIsPressed),
                                                          x(parent.x),
                                                          y(parent.y){}

            Type get_type() const noexcept { return type; }
            std::pair<int,int> get_pos() const noexcept { return {x,y}; }
            int get_x() const noexcept { return x; }
            int get_y() const noexcept { return x; }
            bool is_left_pressed() const noexcept { return leftIsPressed; }
            bool is_right_pressed() const noexcept { return rightIsPressed; }
    };

public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator = (const Mouse&) = delete;

    std::optional<RawDelta> read_raw_delta() noexcept;

    std::pair<int,int> get_pos() const noexcept;
    int get_x() const noexcept;
    int get_y() const noexcept;

    int get_wheel_delta_carry() const noexcept;

    bool is_in_window() const noexcept;
    bool left_is_pressed() const noexcept;
    bool right_is_pressed() const noexcept;

    void enable_raw() noexcept;
    void disable_raw() noexcept;
    bool raw_enabled() const noexcept;

    std::optional<Mouse::Event> read() noexcept;

    bool is_empty() const noexcept;

    void clear() noexcept;

public:
    void on_mouse_move(int x, int y) noexcept;
    void on_mouse_enter() noexcept;
    void on_mouse_leave() noexcept;
    void on_left_press(int x, int y) noexcept;
    void on_right_press(int x, int y) noexcept;
    void on_left_release(int x, int y) noexcept;
    void on_right_release(int x, int y) noexcept;
    void on_wheel_up(int x, int y) noexcept;
    void on_wheel_down(int x, int y) noexcept;
    void on_wheel_delta(int x, int y, int delta) noexcept;
    void on_raw_delta(int dx, int dy) noexcept;

private:
    void trim_buffer() noexcept;
    void trim_raw_buffer() noexcept;

private:
    static constexpr unsigned int bufferSize = 16u;
    static constexpr unsigned int WHEEL_DELTA = 120u;

    int x;
    int y;

    bool leftIsPressed = false;
    bool rightIsPressed = false;
    bool isInWindow = false;
    bool rawEnabled = false;

    int wheelDeltaCarry = 0;

    std::queue<Event> buffer;
    std::queue<RawDelta> rawDeltaBuffer;

};