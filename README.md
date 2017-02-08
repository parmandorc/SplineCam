# SplineCam ([Video demo](https://youtu.be/T_a2e2jTyLA))

### Mode

You can use the number keys to change between modes:

1. Free Cam mode. Simply move the camera through the scene.
2. Spline Editor mode. Edit the control points that define the spline.
3. Spline Follow mode. The camera automatically travels along the created spline.

### Camera controls

- `W` and `S` keys to move forwards and backwards.
- `A` and `D` keys to move sideways.
- `Q` and `E` keys to move up and down.
- `Arrow keys` to rotate the camera.

Only in _Spline Editor_ mode:

- `F` key to focus the camera on the animated indicator (in blue).
- `Shift + F` to focus the camera on the selected control point (in yellow).

### Spline edition controls

Use the same controls that move the camera while holding down the left shift key to move the currently selected control point (in yellow):

- `Shift + W` and `Shift + S` to move the control point forwards and backwards.
- `Shift + A` and `Shift + D` to move the control point sideways.
- `Shift + Q` and `Shift + E` to move the control point up and down.
- `Shift + arrow keys` to rotate the control point.

Other controls for spline edition:
- `Tab` key (and `Shift + Tab`) to select other control points.
- `Enter` key to create a new control point at the current position of the animated indicator (in blue).
    - `Shift + enter` to record the current position and orientation of the camera to create a control point at the position of the animated indicator (in blue).
- `Backspace`key to delete the selected control point.
    - `Shift + backspace` to delete the orientation of the selected control point (so the tangent of the spline is used instead).
- `F1` key to toggle wireframe rendering.
- `F2` key to toggle debug points (explicit rendering of the points that are used to draw the spline).
- `F3` key to print control points and spline length to the console.
- `F4` key to toggle the spline between clamped and cyclic.
- While in _Spline Follow_ mode, `Enter` key to toggle rendering of the spline.

### Playback controls
- `Space` key to stop and resume the animation.
- `Z` key to rewind / play the animation backwards.
- `X` key to fastforward or, if paused, to manually advance the animation.
