Scene: Example
Entities:
  - Entity: f68d0b0e-a927-4bcf-99b7-98478b4b213f
    IdentityComponent:
      Name: BlueCube
    TransformComponent:
      Position: [2.13558698, -0.503358483, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    SpriteComponent:
      Color: [1, 1, 1, 1]
      TexturePath: res/Assets\Images\Pong\Ball.png
    ParticleComponent:
      Rate: 1
      Direction: [2.5999999, -1, 0]
      DirectionVelocity: [0, 1, 0]
      SizeStart: [0.5, 0.5, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [0, 0.400000006, 1, 1]
      ColorEnd: [0, 0.400000006, 1, 1]
      ColorChangeSpeed: 2
      Lifetime: 2
  - Entity: c40e71f7-9a17-459f-ae7b-f81f2359f771
    IdentityComponent:
      Name: RedCube
    TransformComponent:
      Position: [1.73598528, -1.26582265, 0]
      Rotation: [0, 0, -0.373500437]
      Size: [4.37614441, 1, 1]
    SpriteComponent:
      Color: [1, 0, 0, 1]
      TexturePath: "null"
  - Entity: 3ea50401-f327-49e9-8ce7-d55fa594f5fd
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0.920000017, 0.370000005, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 3.5999999