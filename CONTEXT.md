# Context

Glossary for the 8080 emulator's display.

## Terms

**VRAM**:
The 7 KB block of memory the game draws into, one bit per pixel. It is laid out for a CRT mounted on its side, so it must be rotated to look upright.

**Game screen**:
The cabinet's native picture: 224 pixels wide by 256 tall, upright.

**Window**:
The host window the game screen is shown in. It is the game screen enlarged by the display scale (currently 3×).

**Bezel**:
The black border around the curved, rounded-corner picture, imitating the edge of the cabinet's tube. It is drawn as part of the picture, so the window itself stays rectangular.

**Phosphor persistence**:
The short fading trail left behind by bright, moving pixels, like a real CRT's afterglow.

**Input port**:
A numbered channel the game reads (with `IN`) to learn what the player is pressing: coin, start buttons, and left, right and fire for each player.

**Shift register**:
Cabinet hardware the game uses to position sprites at any horizontal pixel offset. The game writes data and an offset to it, then reads back the shifted result.

**Coin**:
The credit the player must insert before a game can be started.
