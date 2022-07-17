#ifndef CIRI_EVENT_H
#define CIRI_EVENT_H

namespace Ciri
{
    enum class EventType
    {
        NONE = 0,
        KEY_PRESS, KEY_RELEASE, KEY_REPEAT,
        MOUSE_PRESS, MOUSE_RELEASE,
        MOUSE_POSITION, MOUSE_SCROLL
    };

    class Event
    {
    private:
        EventType m_EventType = EventType::NONE;

    public:
        Event(EventType type) : m_EventType(type) {}

        EventType GetEventType() const { return m_EventType; }
    };

    class KeyEvent : public Event
    {
    private:
        int m_Key;

    public:
        KeyEvent(EventType type, int key) : Event(type), m_Key(key) {}

        int GetKey() { return m_Key; }
    };

    class MouseButtonEvent : public Event
    {
    private:
        int m_Button;

    public:
        MouseButtonEvent(EventType type, int button) : Event(type), m_Button(button) {}

        int GetButton() { return m_Button; }
    };

    class MousePositionEvent : public Event
    {
    private:
        double m_XPos, m_YPos;

    public:
        MousePositionEvent(double xpos, double ypos) : Event(EventType::MOUSE_POSITION), m_XPos(xpos), m_YPos(ypos) {}

        double GetXPos() { return m_XPos; }
        double GetYPos() { return m_YPos; }
    };

    class MouseScrollEvent : public Event
    {
    private:
        double m_XOffset, m_YOffset;

    public:
        MouseScrollEvent(double xoffset, double yoffset) : Event(EventType::MOUSE_SCROLL), m_XOffset(xoffset), m_YOffset(yoffset) {}

        double GetXOffset() { return m_XOffset; }
        double GetYOffset() { return m_YOffset; }
    };
}

#endif