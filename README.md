# Noise Workshop Resonate 2015

The project files for my Noise Workshop at Resonate 2015. 

This repository contains cheat sheets for the things we will program from scratch.
https://github.com/andreasmuller/NoiseWorkshopScratchpad/

Some projects require ofxAutoReloadedShader, the oF 0.8.4 compatible version:
https://github.com/andreasmuller/ofxAutoReloadedShader/tree/master_0.8.4

The Kinect project requires ofxCv, the oF 0.8.4 compatible version:
https://github.com/kylemcdonald/ofxCv/tree/0.8.4

At the workshop we went through things in this order:

- We started with the Empty template and turned it into NoiseWorkshopScratchpad/Frequency
- StepFunctions
- ShapingFunctions
- Thresholding
- Empty3D -> NoiseWorkshopScratchpad/NoiseTerrain
- Empty3D -> NoiseWorkshopScratchpadNoiseGrid3D
- NoiseGridParticles
- LiveCodingSurface, focus on bin/data/Shaders/LiveCodingSurface/GL2/LiveCodingSurface.frag
- PerlinClouds, focus on bin/data/Shaders/RayMarchingCloudsVolumeofCamera.frag
- Grass
- Tendrils
- EmptyGPUParticles -> NoiseWorkshopScratchpad/ParticlesGPU
- A little jump to ParticleCloudGPU, just a refactored version of what we just built, with some simple additions that make it prettier. Play with making motion in bin/data/Shaders/Particles/GL2/Update.frag
- Your result from the GPU particles we made from scratch -> NoiseWorkshopScratchpad/ParticlesGPUInstancedGeometry
- A little jump again to ParticleSystemInstancedGPU
- ParticleSystemSpawnTexture, introduces the idea of filling up a texture with potential spawn positions for the particles every frame.
- KinectParticles 


<br>
<br>

![Alt text](Screenshots/PosterImageSmall.jpg?raw=true "Optional Title")
