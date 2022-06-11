Scene: WaterFountain
Entities:
  - Entity: d7650c6a-fdd0-4354-9c93-8a7e787c9c4d
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [0, -3.9000001, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.5
      MaxParticles: 5000
      BurstMode: false
      Rate: 3
      Lifetime: 1.5
      Direction: [0, 6, 0]
      DirectionVelocity: [6.5, 1, 0]
      SizeStart: [0.5, 0.5, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [0.563706577, 0.863553226, 1, 1]
      ColorEnd: [0.0154440403, 0.409267008, 1, 1]
      ColorChangeSpeed: 1
      UsePhysics: true
      Gravity: -9.80000019
      Mass: 1
  - Entity: bd7a851b-2b79-428b-b23f-68c7fe599793
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0, -1.10000002, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 3.9000001
  - Entity: 3b4da177-a326-4189-be85-86868d31691d
    IdentityComponent:
      Name: Entity
    TransformComponent:
      Position: [0, -4.5, 0]
      Rotation: [0, 0, 0]
      Size: [9.60000038, 1, 1]
    SpriteComponent:
      Color: [0.65637064, 0.65637064, 0.65637064, 1]
      TexturePath: "null"