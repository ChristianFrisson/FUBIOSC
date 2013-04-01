using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Fubi_WPF_GUI
{
    class MouseSimulator
    {
        // Dll import from user32.dll for the C-API SendInput function
        [DllImport("user32.dll", SetLastError = true)]
        static extern UInt32 SendInput(UInt32 numberOfInputs, INPUT[] inputs, Int32 sizeOfInputStructure);

        // And a wrapper for it
        static bool sendInput(INPUT input)
        {
            INPUT[] inputList = new INPUT[1];
            inputList[0] = input;

            var numberOfSuccessfulSimulatedInputs = SendInput(1, inputList, Marshal.SizeOf(typeof(INPUT)));
            return numberOfSuccessfulSimulatedInputs > 0;
        }

        public static bool sendMousePos(double x, double y)
        {
            Int32 dx = (Int32)(x*65535);
            Int32 dy = (Int32)(y*65535);

            INPUT input = new INPUT();
            input.Type = (UInt32)InputType.MOUSE;
            input.Data.Mouse = new MOUSEINPUT();
            input.Data.Mouse.X = dx;
            input.Data.Mouse.Y = dy;
            input.Data.Mouse.Flags = (uint)(MouseSimulator.MouseFlag.ABSOLUTE | MouseSimulator.MouseFlag.MOVE);

            return sendInput(input);
        }

        public static bool sendMouseButton(bool left, bool down)
        {
            INPUT input = new INPUT();
            input.Type = (UInt32)InputType.MOUSE;
            if (left)
            {
                if (down)
                    input.Data.Mouse.Flags = (uint)MouseFlag.LEFTDOWN;
                else
                    input.Data.Mouse.Flags = (uint)MouseFlag.LEFTUP;
            }
            else
            {
                if (down)
                    input.Data.Mouse.Flags = (uint)MouseFlag.RIGHTDOWN;
                else
                    input.Data.Mouse.Flags = (uint)MouseFlag.RIGHTUP;
            }

            return sendInput(input);
        }

        enum InputType : uint // UInt32
        {
            /// <summary>
            /// INPUT_MOUSE = 0x00 (The event is a mouse event. Use the mi structure of the union.)
            /// </summary>
            MOUSE = 0,

            /// <summary>
            /// INPUT_KEYBOARD = 0x01 (The event is a keyboard event. Use the ki structure of the union.)
            /// </summary>
            KEYBOARD = 1,

            /// <summary>
            /// INPUT_HARDWARE = 0x02 (Windows 95/98/Me: The event is from input hardware other than a keyboard or mouse. Use the hi structure of the union.)
            /// </summary>
            HARDWARE = 2,
        }

        struct MOUSEINPUT
        {
            /// <summary>
            /// Specifies the absolute position of the mouse, or the amount of motion since the last mouse event was generated, depending on the value of the dwFlags member. Absolute data is specified as the x coordinate of the mouse; relative data is specified as the number of pixels moved. 
            /// </summary>
            public Int32 X;

            /// <summary>
            /// Specifies the absolute position of the mouse, or the amount of motion since the last mouse event was generated, depending on the value of the dwFlags member. Absolute data is specified as the y coordinate of the mouse; relative data is specified as the number of pixels moved. 
            /// </summary>
            public Int32 Y;

            /// <summary>
            /// If dwFlags contains MOUSEEVENTF_WHEEL, then mouseData specifies the amount of wheel movement. A positive value indicates that the wheel was rotated forward, away from the user; a negative value indicates that the wheel was rotated backward, toward the user. One wheel click is defined as WHEEL_DELTA, which is 120. 
            /// Windows Vista: If dwFlags contains MOUSEEVENTF_HWHEEL, then dwData specifies the amount of wheel movement. A positive value indicates that the wheel was rotated to the right; a negative value indicates that the wheel was rotated to the left. One wheel click is defined as WHEEL_DELTA, which is 120.
            /// Windows 2000/XP: IfdwFlags does not contain MOUSEEVENTF_WHEEL, MOUSEEVENTF_XDOWN, or MOUSEEVENTF_XUP, then mouseData should be zero. 
            /// If dwFlags contains MOUSEEVENTF_XDOWN or MOUSEEVENTF_XUP, then mouseData specifies which X buttons were pressed or released. This value may be any combination of the following flags. 
            /// </summary>
            public UInt32 MouseData;

            /// <summary>
            /// A set of bit flags that specify various aspects of mouse motion and button clicks. The bits in this member can be any reasonable combination of the following values. 
            /// The bit flags that specify mouse button status are set to indicate changes in status, not ongoing conditions. For example, if the left mouse button is pressed and held down, MOUSEEVENTF_LEFTDOWN is set when the left button is first pressed, but not for subsequent motions. Similarly, MOUSEEVENTF_LEFTUP is set only when the button is first released. 
            /// You cannot specify both the MOUSEEVENTF_WHEEL flag and either MOUSEEVENTF_XDOWN or MOUSEEVENTF_XUP flags simultaneously in the dwFlags parameter, because they both require use of the mouseData field. 
            /// </summary>
            public UInt32 Flags;

            /// <summary>
            /// Time stamp for the event, in milliseconds. If this parameter is 0, the system will provide its own time stamp. 
            /// </summary>
            public UInt32 Time;

            /// <summary>
            /// Specifies an additional value associated with the mouse event. An application calls GetMessageExtraInfo to obtain this extra information. 
            /// </summary>
            public IntPtr ExtraInfo;
        }

        struct HARDWAREINPUT
        {
            /// <summary>
            /// Value specifying the message generated by the input hardware. 
            /// </summary>
            public UInt32 Msg;

            /// <summary>
            /// Specifies the low-order word of the lParam parameter for uMsg. 
            /// </summary>
            public UInt16 ParamL;

            /// <summary>
            /// Specifies the high-order word of the lParam parameter for uMsg. 
            /// </summary>
            public UInt16 ParamH;
        }

        [StructLayout(LayoutKind.Explicit)]
        struct MOUSEKEYBDHARDWAREINPUT
        {
            [FieldOffset(0)]
            public MOUSEINPUT Mouse;

            [FieldOffset(0)]
            public KEYBDINPUT Keyboard;

            [FieldOffset(0)]
            public HARDWAREINPUT Hardware;
        }
        
        struct KEYBDINPUT
        {
            /// <summary>
            /// Specifies a virtual-key code. The code must be a value in the range 1 to 254. The Winuser.h header file provides macro definitions (VK_*) for each value. If the dwFlags member specifies KEYEVENTF_UNICODE, wVk must be 0. 
            /// </summary>
            public UInt16 Vk;

            /// <summary>
            /// Specifies a hardware scan code for the key. If dwFlags specifies KEYEVENTF_UNICODE, wScan specifies a Unicode character which is to be sent to the foreground application. 
            /// </summary>
            public UInt16 Scan;

            /// <summary>
            /// Specifies various aspects of a keystroke. This member can be certain combinations of the following values.
            /// KEYEVENTF_EXTENDEDKEY - If specified, the scan code was preceded by a prefix byte that has the value 0xE0 (224).
            /// KEYEVENTF_KEYUP - If specified, the key is being released. If not specified, the key is being pressed.
            /// KEYEVENTF_SCANCODE - If specified, wScan identifies the key and wVk is ignored. 
            /// KEYEVENTF_UNICODE - Windows 2000/XP: If specified, the system synthesizes a VK_PACKET keystroke. The wVk parameter must be zero. This flag can only be combined with the KEYEVENTF_KEYUP flag. For more information, see the Remarks section. 
            /// </summary>
            public UInt32 Flags;

            /// <summary>
            /// Time stamp for the event, in milliseconds. If this parameter is zero, the system will provide its own time stamp. 
            /// </summary>
            public UInt32 Time;

            /// <summary>
            /// Specifies an additional value associated with the keystroke. Use the GetMessageExtraInfo function to obtain this information. 
            /// </summary>
            public IntPtr ExtraInfo;
        }

        struct INPUT
        {
            /// <summary>
            /// Specifies the type of the input event. This member can be one of the following values. 
            /// InputType.MOUSE - The event is a mouse event. Use the mi structure of the union.
            /// InputType.KEYBOARD - The event is a keyboard event. Use the ki structure of the union.
            /// InputType.HARDWARE - Windows 95/98/Me: The event is from input hardware other than a keyboard or mouse. Use the hi structure of the union.
            /// </summary>
            public UInt32 Type;

            /// <summary>
            /// The data structure that contains information about the simulated Mouse, Keyboard or Hardware event.
            /// </summary>
            public MOUSEKEYBDHARDWAREINPUT Data;
        }

        enum MouseFlag : uint
        {
            /// <summary>
            /// Specifies that movement occurred.
            /// </summary>
            MOVE = 0x0001,

            /// <summary>
            /// Specifies that the left button was pressed.
            /// </summary>
            LEFTDOWN = 0x0002,

            /// <summary>
            /// Specifies that the left button was released.
            /// </summary>
            LEFTUP = 0x0004,

            /// <summary>
            /// Specifies that the right button was pressed.
            /// </summary>
            RIGHTDOWN = 0x0008,

            /// <summary>
            /// Specifies that the right button was released.
            /// </summary>
            RIGHTUP = 0x0010,

            /// <summary>
            /// Specifies that the middle button was pressed.
            /// </summary>
            MIDDLEDOWN = 0x0020,

            /// <summary>
            /// Specifies that the middle button was released.
            /// </summary>
            MIDDLEUP = 0x0040,

            /// <summary>
            /// Windows 2000/XP: Specifies that an X button was pressed.
            /// </summary>
            XDOWN = 0x0080,

            /// <summary>
            /// Windows 2000/XP: Specifies that an X button was released.
            /// </summary>
            XUP = 0x0100,

            /// <summary>
            /// Windows NT/2000/XP: Specifies that the wheel was moved, if the mouse has a wheel. The amount of movement is specified in mouseData. 
            /// </summary>
            WHEEL = 0x0800,

            /// <summary>
            /// Windows 2000/XP: Maps coordinates to the entire desktop. Must be used with MOUSEEVENTF_ABSOLUTE.
            /// </summary>
            VIRTUALDESK = 0x4000,

            /// <summary>
            /// Specifies that the dx and dy members contain normalized absolute coordinates. If the flag is not set, dxand dy contain relative data (the change in position since the last reported position). This flag can be set, or not set, regardless of what kind of mouse or other pointing device, if any, is connected to the system. For further information about relative mouse motion, see the following Remarks section.
            /// </summary>
            ABSOLUTE = 0x8000,
        }
    }
}
