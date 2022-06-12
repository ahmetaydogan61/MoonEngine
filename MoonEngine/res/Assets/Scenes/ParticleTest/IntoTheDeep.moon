Scene: IntoTheDeep
Entities:
  - Entity: 07d3be4d-539f-4349-b457-3060d1e9ef69
    IdentityComponent:
      Name: Entity
    TransformComponent:
      Position: [0, -0.699999988, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    SpriteComponent:
      Color: [1, 1, 1, 1]
      TexturePath: res/Assets\Images\Pong\Ball.png
  - Entity: 955df302-b063-4d7d-863a-0a4cfe08c079
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [0, -0.5, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 5000
      BurstMode: false
      Rate: 5
      Lifetime: 2
      Direction: [0, 5.19999981, 0]
      DirectionVelocity: [0.899999976, 1, 0]
      ShapeSize: [0, 0, 0]
      SizeStart: [0.5, 0.5, 0]
      SizeEnd: [0, 0, 0]
      SizeChangeSpeed: 1
      TexturePath: "null"
      ColorStart: [0, 0.501930714, 1, 1]
      ColorEnd: [1, 0, 0, 1]
      ColorChangeSpeed: 1
      UsePhysics: false
      Gravity: -9.80000019
      Mass: 1
  - Entity: 0dc193de-a592-4466-9818-540b9a37a7f4
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [5, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 5000
      BurstMode: false
      Rate: 5
      Lifetime: 7
      Direction: [0, 5.4000001, 0]
      DirectionVelocity: [0, 10, 0]
      ShapeSize: [4, 5.0999999, 0]
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
  - Entity: f6a14e84-71d8-445f-a3cd-cac58ec5a38b
    IdentityComponent:
      Name: Particle
    TransformComponent:
      Position: [-5, 0, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    ParticleComponent:
      Play: true
      AutoPlay: true
      Duration: 0.75
      MaxParticles: 5000
      BurstMode: false
      Rate: 5
      Lifetime: 7
      Direction: [0, 5.4000001, 0]
      DirectionVelocity: [0, 10, 0]
      ShapeSize: [4, 5.0999999, 0]
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
  - Entity: 05b3ef5f-2eff-4b7d-941f-3b07c24b0ec1
    IdentityComponent:
      Name: Camera
    TransformComponent:
      Position: [0, 2.9000001, 0]
      Rotation: [0, 0, 0]
      Size: [1, 1, 1]
    CameraComponent:
      IsMain: true
      Distance: 4