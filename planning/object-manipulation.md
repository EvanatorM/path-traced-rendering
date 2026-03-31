# Object deletion
- Raycast
- Colliders
    - Cube
    - Sphere
    - Plane
- Destroy object that was hit by raycast
    - Don't allow destruction of planes
- Reset image

# Object creation
- Object creation parameters changed in ImGui
    - Object type (sphere, cube)
    - Object size
    - Material ID (dropdown)
- Raycast
    - Get offset based on object size
- Add object to scene
- Reset image

# Other changes
- Only reupload buffers when the scene changes
    - Scene onDirty flag