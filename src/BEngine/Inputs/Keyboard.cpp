#include "Keyboard.hpp"

bool Keyboard::KeyIsPressed(int keycode) const noexcept
{
    return  keystates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
    if(keybuffer.size() > 0u)
    {
        Keyboard::Event event = keybuffer.front();
        keybuffer.pop();
        return event;
    }
    return {};
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return keybuffer.empty();
}

void Keyboard::ClearKeyQueue() noexcept
{
    std::queue<Event> empty;
    std::swap(keybuffer, empty);
}

void Keyboard::Clear() noexcept
{
    ClearKeyQueue();
    // clear text input queue if needed..
}

void Keyboard::EnableAutorepeat() noexcept
{
    autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
    autorepeatEnabled = false;
}


bool Keyboard::AutorepeatIsEnabled() const noexcept
{
    return autorepeatEnabled;
}

template<class K>
void Keyboard::TrimBuffer(std::queue<K>& buffer) noexcept
{
    while(buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}

void Keyboard::OnKeyPressed(int keycode) noexcept
{
    keystates[keycode] = true;
    keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
    TrimBuffer(keybuffer);
}

void Keyboard::OnKeyReleased(int keycode) noexcept
{
    keystates[keycode] = false;
    keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
    TrimBuffer(keybuffer);
}

void Keyboard::ClearState() noexcept
{
    keystates.reset();
}

