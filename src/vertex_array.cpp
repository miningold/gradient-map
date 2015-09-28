#include "vertex_array.h"
#include "common.h"
#include "buffer_object.h"

namespace knight {

VertexArray::VertexArray() {
  GL(glGenVertexArrays(1, &handle_));
}

VertexArray::VertexArray(VertexArray &&other) :
    handle_{other.handle_} {
  other.handle_ = 0;
}

VertexArray &VertexArray::operator=(VertexArray &&other) {
  handle_ = other.handle_;
  other.handle_ = 0;
  return *this;
}

VertexArray::~VertexArray() {
  if (handle_) {
    glDeleteVertexArrays(1, &handle_);
  }
}

void VertexArray::Bind() const {
  XASSERT(handle_, "Trying to bind an uninitialized vertex array");
  GL(glBindVertexArray(handle_));
}

void VertexArray::Unbind() const {
  auto vertex_array_binding = GLint{};
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vertex_array_binding);
  if (handle_ == (GLuint)vertex_array_binding) {
    glBindVertexArray(0);
  }
}

void VertexArray::BindAttribute(BufferObject &buffer, GLint attribute, GLint size, GLenum type,
                                GLboolean normalized, GLsizei stride, GLintptr offset) {
  Bind();
  buffer.Bind();
  GL(glEnableVertexAttribArray(attribute));
  GL(glVertexAttribPointer(attribute, size, type, normalized, stride, reinterpret_cast<const GLvoid*>(offset)));
}

} // namespace knight
