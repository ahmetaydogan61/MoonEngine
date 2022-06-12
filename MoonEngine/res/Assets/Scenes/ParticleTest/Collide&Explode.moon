Scene: "Collide&Explode"
Entities:
  - Entity: e7373ba1-c6c1-4527-85b8-e92b225269f1
    IdentityComponent:
      Name: Explosion
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 1.20000005
      MaxParticles: 5000
      BurstMode: true
      Rate: 125
      Lifetime: 3
      Direction: [0, 0, 0]
      DirectionVelocity: [3, 3, 0]
      ShapeSize: [1, 0, 0]
      SizeStart: [0.5, 0.5, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [1, 1, 1, 1]
      ColorEnd: [0, 0.400000006, 1, 1]
      ColorChangeSpeed: 1
      UsePhysics: false
      Gravity: -9.80000019
      Mass: 1
  - Entity: 09576111-be36-46ff-8ae4-f1e872dc9067
    IdentityComponent:
      Name: DownParticle
    TransformComponent:
      Position: [0, -7.5, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 1
      MaxParticles: 5000
      BurstMode: true
      Rate: 250
      Lifetime: 1.5
      Direction: [0, -2.5, 0]
      DirectionVelocity: [2, 1, 0]
      ShapeSize: [0, 0, 0]
      SizeStart: [1, 1, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: "null"
      ColorStart: [1, 1, 1, 1]
      ColorEnd: [0, 0.400000006, 1, 1]
      ColorChangeSpeed: 1
      UsePhysics: true
      Gravity: 9.80000019
      Mass: 1
  - Entity: 05b3ef5f-2eff-4b7d-941f-3b07c24b0ec1
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 4
  - Entity: 961b1cc6-acb7-4b27-9349-c388396e308b
    IdentityComponent:
      Name: UpParticle
    TransformComponent:
      Position: [0, 7.5, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 1
      MaxParticles: 5000
      BurstMode: true
      Rate: 250
      Lifetime: 1.5
      Direction: [0, 2.5, 0]
      DirectionVelocity: [2, 1, 0]
      ShapeSize: [0, 0, 0]
      SizeStart: [1, 1, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: "null"
      ColorStart: [1, 1, 1, 1]
      ColorEnd: [0, 0.400000006, 1, 1]
      ColorChangeSpeed: 1
      UsePhysics: true
      Gravity: -9.80000019
      Mass: 1