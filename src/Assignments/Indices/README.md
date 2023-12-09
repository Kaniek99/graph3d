We reduced the number of vertices by 4. Each vertex is represented by 7 values of type *GLfloat*. So we freed up *4\*7\*sizeof(GLfloat)* where *sizeof(GLfloat) = 4*.

After quick calculation we get 112. Note that this involved creation of a vectror of variables of type GLushort and variable VBO_indexes of type GLuint.

*sizeof(GLushort) = 2, sizeof(GLuint) = 4* so we additionally used *9\*sizeof(GLushort)+sizeof(GLuint)=22* bytes. **The final result of the freed memory is 112-22=90 bytes.**

If the colors are irrelevant (as in this case) we can improve this result by using *GL_TRIANGLES_FAN* instead of *GL_TRIANGLE*. Then we need 5 vertices but we don't need variables

associated with indices.