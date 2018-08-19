

class SSAORenderer
{
private:
  uint SSAOTexture;
  std::vector<glm::vec3> ssaoNoise;
  std::vector<glm::vec3> ssaoKernel;
  uint SSAOFBO, SSAOColorBuffer;
  uint SSAOBlurFBO, SSAOColorBufferBlur;
  Shader SSAOShader,SSAOBlurShader;
public:
  SSAORenderer();
}
