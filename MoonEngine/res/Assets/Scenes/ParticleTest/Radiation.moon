Scene: Radiation
Entities:
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
  - Entity: 9c8df22a-eca7-4b86-bdeb-09fccf9731e0
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      MaxParticles: 100000
      Rate: 5
      Lifetime: 2
      Direction: [0, 0, 0]
      DirectionVelocity: [1, 1, 0]
      SizeStart: [0.5, 0.5, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [0.216216207, 0.15610978, 0.15610978, 1]
      ColorEnd: [0.262745112, 0.721568644, 0.13333334, 0.843137264]
      ColorChangeSpeed: 2
  - Entity: 255960aa-9624-4ddf-8525-d378f6bd89c6
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      MaxParticles: 50000
      Rate: 10
      Lifetime: 2
      Direction: [0, 0, 0]
      DirectionVelocity: [1, 1, 0]
      SizeStart: [1, 1, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: res/Assets\Images\Pong\Ball.png
      ColorStart: [0, 0, 0, 1]
      ColorEnd: [0, 0, 0, 0.721568644]
      ColorChangeSpeed: 1