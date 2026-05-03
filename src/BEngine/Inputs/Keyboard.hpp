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

            bool IsPressed() const noexcept { return type == Type::Press; }
            bool IsReleased() const noexcept { return type == Type::Release; }
            bool IsValid() const noexcept { return type != Type::Invalid; }

            int GetCode() const noexcept { return code; }
    };

public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;


    bool KeyIsPressed(int) const noexcept;
    std::optional<Event> ReadKey() noexcept;
    bool KeyIsEmpty() const noexcept;
    void ClearKeyQueue() noexcept;


    void Clear() noexcept;

    // autorepeat controls
    void EnableAutorepeat() noexcept;
    void DisableAutorepeat() noexcept;
    bool AutorepeatIsEnabled() const noexcept;

public:
    void OnKeyPressed(int) noexcept;
    void OnKeyReleased(int) noexcept;

    // clears the keystates
    void ClearState() noexcept;

    template<class K>
    static void TrimBuffer(std::queue<K>&) noexcept;

private:

    static constexpr unsigned int nKeys = 265u; // number of virtual keycodes
    static constexpr unsigned int bufferSize = 16u;

    bool autorepeatEnabled = true;
    
    std::bitset<nKeys> keystates;
    std::queue<Event> keybuffer;


};