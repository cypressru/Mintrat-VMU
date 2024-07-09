//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// StyleAsCode exporter v2.0 - Style data exported as a values array            //
//                                                                              //
// USAGE: On init call: GuiLoadStyleMintedRat();                                //
//                                                                              //
// more info and bugs-report:  github.com/raysan5/raygui                        //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2020-2023 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define MINTEDRAT_STYLE_PROPS_COUNT 14

// Custom style name: MintedRat
static const GuiStyleProp mintedratStyleProps[MINTEDRAT_STYLE_PROPS_COUNT] = {
    {0, 0, 0x000201ff},   // DEFAULT_BORDER_COLOR_NORMAL
    {0, 1, 0x7efcb2ff},   // DEFAULT_BASE_COLOR_NORMAL
    {0, 2, 0x000201ff},   // DEFAULT_TEXT_COLOR_NORMAL
    {0, 3, 0xf4fc00ff},   // DEFAULT_BORDER_COLOR_FOCUSED
    {0, 4, 0x020200ff},   // DEFAULT_BASE_COLOR_FOCUSED
    {0, 5, 0xf4fc00ff},   // DEFAULT_TEXT_COLOR_FOCUSED
    {0, 6, 0x7e97fcff},   // DEFAULT_BORDER_COLOR_PRESSED
    {0, 7, 0x000102ff},   // DEFAULT_BASE_COLOR_PRESSED
    {0, 8, 0x7e97fcff},   // DEFAULT_TEXT_COLOR_PRESSED
    {0, 9, 0x020202ff},   // DEFAULT_BORDER_COLOR_DISABLED
    {0, 10, 0xc9fcdeff},  // DEFAULT_BASE_COLOR_DISABLED
    {0, 11, 0x020202ff},  // DEFAULT_TEXT_COLOR_DISABLED
    {0, 18, 0x000201ff},  // DEFAULT_LINE_COLOR
    {0, 19, 0x7efcb2ff},  // DEFAULT_BACKGROUND_COLOR
};

// Style loading function: MintedRat
static void GuiLoadStyleMintedRat(void) {
    // Load style properties provided
    // NOTE: Default properties are propagated
    for (int i = 0; i < MINTEDRAT_STYLE_PROPS_COUNT; i++) {
        GuiSetStyle(mintedratStyleProps[i].controlId, mintedratStyleProps[i].propertyId, mintedratStyleProps[i].propertyValue);
    }
}
