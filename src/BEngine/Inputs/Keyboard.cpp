#include "Keyboard.hpp"

bool Keyboard::key_is_pressed(int keycode) const noexcept { return  keystates[keycode]; }

std::optional<Keyboard::Event> Keyboard::read_key() noexcept
{
    if(keybuffer.size() > 0u)
    {
        Keyboard::Event event = keybuffer.front();
        keybuffer.pop();
        return event;
    }
    return {};
}

bool Keyboard::key_is_empty() const noexcept { return keybuffer.empty(); }

void Keyboard::clear_key_queue() noexcept { while (!keybuffer.empty()) keybuffer.pop(); }

void Keyboard::clear() noexcept
{
    clear_key_queue();
    // clear text input queue if needed..
}

void Keyboard::enable_autorepeat() noexcept { autorepeatEnabled = true; }

void Keyboard::disable_autorepeat() noexcept { autorepeatEnabled = false; }


bool Keyboard::autorepeat_is_enabled() const noexcept { return autorepeatEnabled; }

template<class K>
void Keyboard::trim_buffer(std::queue<K>& buffer) noexcept
{
    while(buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}

void Keyboard::on_key_pressed(int keycode) noexcept
{
    keystates[keycode] = true;
    keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
    trim_buffer(keybuffer);
}

void Keyboard::on_key_released(int keycode) noexcept
{
    keystates[keycode] = false;
    keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
    trim_buffer(keybuffer);
}

void Keyboard::clear_state() noexcept { keystates.reset(); }

