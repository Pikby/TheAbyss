SSAORenderer::SSAORenderer()
{
  SSAOShader = Shader("../src/Shaders/SSAOShaders/SSAO.fs","../src/Shaders/SSAOShaders/SSAO.vs");
  SSAOShader.use();
  SSAOShader.setInt("gPosition",0);
  SSAOShader.setInt("gNormal",1);
  SSAOShader.setInt("texNoise",2);

  SSAOBlurShader = Shader("../src/Shaders/SSAOShaders/SSAOBlur.fs","../src/Shaders/SSAOShaders/SSAOBlur.vs");
  SSAOBlurShader.use();
  SSAOBlurShader.setInt("ssaoInput",0);
  std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
  std::default_random_engine generator;
  for (unsigned int i = 0; i < 64; ++i)
  {
      glm::vec3 sample(
          randomFloats(generator) * 2.0 - 1.0,
          randomFloats(generator) * 2.0 - 1.0,
          randomFloats(generator)
      );
      sample  = glm::normalize(sample);
      sample *= randomFloats(generator);
      float scale = (float)i / 64.0;

      scale = lerp(0.1f, 1.0f, scale * scale);
      sample *= scale;
      ssaoKernel.push_back(sample);
  }
  for (unsigned int i = 0; i < 16; i++)
  {
      glm::vec3 noise(
          randomFloats(generator) * 2.0 - 1.0,
          randomFloats(generator) * 2.0 - 1.0,
          0.0f);
      ssaoNoise.push_back(noise);
  }
  glGenTextures(1, &SSAOTexture);
  glBindTexture(GL_TEXTURE_2D, SSAOTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


  glGenFramebuffers(1, &SSAOFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);

  glGenTextures(1, &SSAOColorBuffer);
  glBindTexture(GL_TEXTURE_2D, SSAOColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOColorBuffer, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  glGenFramebuffers(1, &SSAOBlurFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFBO);
  glGenTextures(1, &SSAOColorBufferBlur);
  glBindTexture(GL_TEXTURE_2D, SSAOColorBufferBlur);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOColorBufferBlur,0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  SSAOShader.use();
  for (unsigned int i = 0; i < 64; ++i)
     SSAOShader.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
}
