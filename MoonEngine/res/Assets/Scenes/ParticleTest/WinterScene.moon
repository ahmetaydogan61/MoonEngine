Scene: WinterScene
Entities:
  - Entity: 255960aa-9624-4ddf-8525-d378f6bd89c6
    IdentityComponent:
      Name: Snow
    TransformComponent:
      Position: [-0.100000001, 2, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 10000
      BurstMode: false
      Rate: 1
      Lifetime: 6
      Direction: [0, -0.5, 0]
      DirectionVelocity: [0.0500000007, 0, 0]
      ShapeSize: [3.29999995, -0.200000003, 0]
      SizeStart: [0.100000001, 0.100000001, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [0, 0.525490224, 1, 0.137254909]
      ColorEnd: [0.00392156839, 0.896161735, 1, 1]
      ColorChangeSpeed: 2
      UsePhysics: false
      Gravity: -1.20000005
      Mass: 1
  - Entity: 7dbe7a3f-2021-4ca7-9634-830648ac0832
    IdentityComponent:
      Name: Fire
    TransformComponent:
      Position: [0, -0.699999988, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 25000
      BurstMode: false
      Rate: 1
      Lifetime: 1
      Direction: [0, 0.349999994, 0]
      DirectionVelocity: [0.25, 0.5, 0]
      ShapeSize: [0, 0, 0]
      SizeStart: [0.100000001, 0.100000001, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: "null"
      ColorStart: [1, 0.254826188, 0, 1]
      ColorEnd: [1, 0.64864862, 0, 1]
      ColorChangeSpeed: 2
      UsePhysics: false
      Gravity: -9.80000019
      Mass: 1
  - Entity: 478080e8-ae6f-402f-bfa5-ef55970889ad
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 1
  - Entity: 06e94bc7-5c82-4136-9ccf-d133a682980f
    IdentityComponent:
      Name: Torch
    TransformComponent:
      Position: [0, -0.839999974, 0]
      Rotation: [0, 0, 0]
      Size: [0.0799999982, 0.200000003, 1]
    SpriteComponent:
      Color: [0.617760658, 0.271910101, 0, 1]
      TexturePath: "null"