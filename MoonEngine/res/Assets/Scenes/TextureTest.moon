Scene: TextureTest
Entities:
  - Entity: 9db73a48-3a55-40e4-8118-55e7a8445f05
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [1.19005287, 4.67523932e-07, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 5
  - Entity: 5551824e-3d32-4952-be99-c1c163439518
    IdentityComponent:
      Name: Entity
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    SpriteComponent:
      Color: [1, 1, 1, 1]
      TexturePath: res/Assets/Images\Star.png
  - Entity: 5b695ec4-50ca-4095-90ea-e68a09a5581c
    IdentityComponent:
      Name: Entity
    TransformComponent:
      Position: [7.10479546, 4.1740675, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    SpriteComponent:
      Color: [1, 1, 1, 1]
      TexturePath: res/Assets/Images\Pong\Ball.png
  - Entity: bf8eda01-32f3-499e-abeb-a79a347011f9
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [-7.35346317, -4.31616306, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 5000
      BurstMode: false
      Rate: 5
      Lifetime: 1.5
      Direction: [5, 5, 0]
      DirectionVelocity: [1, 1, 0]
      ShapeSize: [0, 0, 0]
      SizeStart: [0.5, 0.5, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets/Images\Pong\Goal.png
      ColorStart: [1, 0, 0, 1]
      ColorEnd: [0, 0.400000006, 1, 1]
      ColorChangeSpeed: 1
      UsePhysics: false
      Gravity: -9.80000019
      Mass: 1