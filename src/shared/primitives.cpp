#include <GL/glew.h>
#include "primitives.h"
#include <iostream>

namespace primitives {
  unsigned int cubeVAO = 0;
  unsigned int cubeVBO = 0;
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;

  void renderCube() {
    if(cubeVAO == 0) {
      glGenVertexArrays(1, &cubeVAO);
      glGenBuffers(1, &cubeVBO);
      glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
      glBindVertexArray(cubeVAO);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
  }

  void renderQuad() {
    if(quadVAO == 0) {
      glGenVertexArrays(1, &quadVAO);
      glGenBuffers(1, &quadVBO);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  }
}
