Scene: TextureText
Entities:
  - Entity: 8db23357-a631-426a-8ef0-dc7bbb94212c
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 5
  - Entity: 992f38ca-a574-4e91-b397-26b6e7debe84
    IdentityComponent:
      Name: Entity
    TransformComponent:
      Position: [2.89156651, 3.87048197, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    SpriteComponent:
      Color: [1, 1, 1, 1]
      TexturePath: res/Assets/Images\Star.png
  - Entity: a6b209aa-428a-43c8-b803-9ac8d856c659
    IdentityComponent:
      Name: Entity
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    SpriteComponent:
      Color: [1, 1, 1, 1]
      TexturePath: res/Assets/Images\Pong\Ball.png
  - Entity: d1bec9c2-0207-40c0-bf2f-fed352e51db4
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [-8.23795223, -4.33734894, 0]
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
      TexturePath: res/Assets/Images\Pong\Player.png
      ColorStart: [1, 0, 0, 1]
      ColorEnd: [0, 0.400000006, 1, 1]
      ColorChangeSpeed: 1
      UsePhysics: false
      Gravity: -9.80000019
      Mass: 1