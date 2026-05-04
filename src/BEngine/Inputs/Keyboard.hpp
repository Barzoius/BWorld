#pragma once

#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{

public:
    class Event
    {
        public:
            enum class Type
            {
                Press,
                Release,
                Invalid
            };     

        private:
            Type type;
            int code;

        public:
            Event() noexcept : type(Type::Invalid), code(0u) {}
            Event(Type type, int code) noexcept : type(type), code(code) {}

            bool is_pressed() const noexcept { return type == Type::Press; }
            bool is_released() const noexcept { return type == Type::Release; }
            bool is_valid() const noexcept { return type != Type::Invalid; }

            int get_code() const noexcept { return code; }
    };

public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;


    bool key_is_pressed(int) const noexcept;
    std::optional<Event> read_key() noexcept;
    bool key_is_empty() const noexcept;
    void clear_key_queue() noexcept;


    void clear() noexcept;

    // autorepeat controls
    void enable_autorepeat() noexcept;
    void disable_autorepeat() noexcept;
    bool autorepeat_is_enabled() const noexcept;

public:
    void on_key_pressed(int) noexcept;
    void on_key_released(int) noexcept;

    // clears the keystates
    void clear_state() noexcept;

    template<class K>
    static void trim_buffer(std::queue<K>&) noexcept;

private:

    static constexpr unsigned int nKeys = 265u; // number of virtual keycodes
    static constexpr unsigned int bufferSize = 16u;

    bool autorepeatEnabled = true;
    
    std::bitset<nKeys> keystates;
    std::queue<Event> keybuffer;


};