

    uniform sampler2D u_particleTexture;
    uniform sampler2D u_spawnTexture;

    uniform vec2 u_resolution;

    uniform float u_deltaTime;
    uniform float u_time;

    uniform float u_persistence;

    const int OCTAVES = 3;
    const float NOISE_POSITION_SCALE = 1.5;
    const float NOISE_SCALE = 0.075;
    const float BASE_SPEED = 0.2;

    void main () 
    {
        vec2 textureCoordinates = gl_FragCoord.xy / u_resolution;
        vec4 data = texture2D(u_particleTexture, textureCoordinates);

        vec3 oldPosition = data.rgb;

        vec3 noisePosition = oldPosition * NOISE_POSITION_SCALE;

        float noiseTime = u_time * NOISE_TIME_SCALE;

        vec4 xNoisePotentialDerivatives = vec4(0.0);
        vec4 yNoisePotentialDerivatives = vec4(0.0);
        vec4 zNoisePotentialDerivatives = vec4(0.0);

        float persistence = u_persistence;

        for (int i = 0; i < OCTAVES; ++i) 
        {
            float scale = (1.0 / 2.0) * pow(2.0, float(i));

            float noiseScale = pow(persistence, float(i));
            if (persistence == 0.0 && i == 0) //fix undefined behaviour
            { 
                noiseScale = 1.0;
            }

            xNoisePotentialDerivatives += simplexNoiseDerivatives(vec4(noisePosition * pow(2.0, float(i)), noiseTime)) * noiseScale * scale;
            yNoisePotentialDerivatives += simplexNoiseDerivatives(vec4((noisePosition + vec3(123.4, 129845.6, -1239.1)) * pow(2.0, float(i)), noiseTime)) * noiseScale * scale;
            zNoisePotentialDerivatives += simplexNoiseDerivatives(vec4((noisePosition + vec3(-9519.0, 9051.0, -123.0)) * pow(2.0, float(i)), noiseTime)) * noiseScale * scale;
        }

        //compute curl
        vec3 noiseVelocity = vec3(
            zNoisePotentialDerivatives[1] - yNoisePotentialDerivatives[2],
            xNoisePotentialDerivatives[2] - zNoisePotentialDerivatives[0],
            yNoisePotentialDerivatives[0] - xNoisePotentialDerivatives[1]
        ) * NOISE_SCALE;

        vec3 velocity = vec3( BASE_SPEED,  0.0, 0.0);
        vec3 totalVelocity = velocity + noiseVelocity;

        vec3 newPosition = oldPosition + totalVelocity * u_deltaTime;

        float oldLifetime = data.a;
        float newLifetime = oldLifetime - u_deltaTime;

        vec4 spawnData = texture2D(u_spawnTexture, textureCoordinates);

        if (newLifetime < 0.0) {
            newPosition = spawnData.rgb;
            newLifetime = spawnData.a + newLifetime;
        }

        gl_FragColor = vec4(newPosition, newLifetime);
    }