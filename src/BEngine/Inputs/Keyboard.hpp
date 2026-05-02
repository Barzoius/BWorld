#pragma once

#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
    friend class Window;

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
            unsigned char code;

        public:
            Event() noexcept : type(Type::Invalid), code(0u) {}
            Event(Type type, unsigned char code) noexcept : type(type), code(code) {}

            bool IsPressed() const noexcept { return type == Type::Press; }
            bool IsReleased() const noexcept { return type == Type::Release; }
            bool IsValid() const noexcept { return type != Type::Invalid; }

            unsigned char GetCode() const noexcept { return code; }
    };

public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;


    bool KeyIsPressed(unsigned char) const noexcept;
    Event ReadKey() noexcept;
    bool KeyIsEmpty() const noexcept;
    void ClearQueue() noexcept;

    char ReadChar() noexcept;
    bool CharIsEmpty() const noexcept;
    void ClearChar() noexcept;

    void Clear() noexcept;

    // autorepeat controls
    void EnableAutorepeat() noexcept;
    void DisableAutorepeat() noexcept;
    bool AutorepeatIsEnabled() const noexcept;

private:
    void OnKeyPressed(unsigned char keycode) noexcept;
    void OnKeyReleased(unsigned char keycode) noexcept;
    void OnChar(char character) noexcept;

    // clears the keystates
    void ClearState() noexcept;

    template<class T>
    static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:

    static constexpr unsigned int nKeys = 265u; // number of virtual keycodes
    static constexpr unsigned int bufferSize = 16u;

    bool autorepeatEnabled = true;
    
    std::bitset<nKeys> keystates;
    std::queue<Event> keybuffer;
    std::queue<char> charbuffer;


    // to be deleted
public:
    void SetKey(int key, bool pressed)
    {
        if (pressed)
            current.set(key);
        else
            current.reset(key);
    }

    bool IsDown(int key) const
    {
        return current.test(key);
    }

    bool WasPressed(int key) const
    {
        return current.test(key) && !previous.test(key);
    }

    void EndFrame()
    {
        previous = current;
    }

    std::bitset<nKeys> current;
    std::bitset<nKeys> previous;

};