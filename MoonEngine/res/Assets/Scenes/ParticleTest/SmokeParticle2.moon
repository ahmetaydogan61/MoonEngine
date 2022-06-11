Scene: SmokeParticle2
Entities:
  - Entity: 478080e8-ae6f-402f-bfa5-ef55970889ad
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0, 1.54999995, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 2.4000001
  - Entity: 255960aa-9624-4ddf-8525-d378f6bd89c6
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [0.800000012, -0.5, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 100000
      BurstMode: false
      Rate: 15
      Lifetime: 5
      Direction: [0, 0.899999976, 0]
      DirectionVelocity: [2, 2.4000001, 0]
      SizeStart: [1, 1, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [0, 0, 0, 1]
      ColorEnd: [0, 0, 0, 0.101960786]
      ColorChangeSpeed: 1
      UsePhysics: true
      Gravity: 1
      Mass: 1