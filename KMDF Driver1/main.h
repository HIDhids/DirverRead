#ifndef _KEYMOUSE_H
#define _KEYMOUSE_H
typedef struct _KEYBOARD_INPUT_DATA {

    USHORT UnitId;

    USHORT MakeCode;

    USHORT Flags;

    USHORT Reserved;

    ULONG ExtraInformation;

} KEYBOARD_INPUT_DATA, * PKEYBOARD_INPUT_DATA;


typedef struct _MOUSE_INPUT_DATA {

    USHORT UnitId;

    USHORT Flags;

    union {
        ULONG Buttons;
        struct {
            USHORT  ButtonFlags;
            USHORT  ButtonData;
        };
    };

    ULONG RawButtons;

    LONG LastX;

    LONG LastY;

    ULONG ExtraInformation;

} MOUSE_INPUT_DATA, * PMOUSE_INPUT_DATA;


#define MOUSE_LEFT_BUTTON        0x0001
#define MOUSE_RIGHT_BUTTON       0x0002
#define MOUSE_LEFT_BUTTON_DOWN   0x0001  // Left Button changed to down.
#define MOUSE_LEFT_BUTTON_UP     0x0002  // Left Button changed to up.
#define MOUSE_RIGHT_BUTTON_DOWN  0x0004  // Right Button changed to down.
#define MOUSE_RIGHT_BUTTON_UP    0x0008  // Right Button changed to up.
#define MOUSE_MIDDLE_BUTTON_DOWN 0x0010  // Middle Button changed to down.
#define MOUSE_MIDDLE_BUTTON_UP   0x0020  // Middle Button changed to up.

#define MOUSE_BUTTON_1_DOWN     MOUSE_LEFT_BUTTON_DOWN
#define MOUSE_BUTTON_1_UP       MOUSE_LEFT_BUTTON_UP
#define MOUSE_BUTTON_2_DOWN     MOUSE_RIGHT_BUTTON_DOWN
#define MOUSE_BUTTON_2_UP       MOUSE_RIGHT_BUTTON_UP
#define MOUSE_BUTTON_3_DOWN     MOUSE_MIDDLE_BUTTON_DOWN
#define MOUSE_BUTTON_3_UP       MOUSE_MIDDLE_BUTTON_UP

#define MOUSE_BUTTON_4_DOWN     0x0040
#define MOUSE_BUTTON_4_UP       0x0080
#define MOUSE_BUTTON_5_DOWN     0x0100
#define MOUSE_BUTTON_5_UP       0x0200

#define MOUSE_WHEEL             0x0400

//
// Define the mouse indicator flags.
//

#define MOUSE_MOVE_RELATIVE         0
#define MOUSE_MOVE_ABSOLUTE         1
#define MOUSE_VIRTUAL_DESKTOP    0x02  // the coordinates are mapped to the virtual desktop
#define MOUSE_ATTRIBUTES_CHANGED 0x04  // requery for mouse attributes



#endif