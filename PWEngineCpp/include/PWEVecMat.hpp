// #pragma once

// #include <cmath>

// namespace PWEngine::Type
// {
    
//     struct PWEMat2
//     {
//         float m[2][2] = {
//             {1,0},
//             {0,1}
//         };

//         static PWEMat2 identity() {
//             return PWEMat2();
//         }

//         static PWEMat2 rotate(float rad) {
//             float c = cos(rad);
//             float s = sin(rad);
//             return {
//                 c, -s,
//                 s,  c
//             };
//         }

//         static PWEMat2 scale(const PWEVec2& s) {
//             return {
//                 s.x, 0,
//                 0, s.y
//             };
//         }

//         PWEMat2 Inverse() const
//         {
//             float det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
//             return {
//                 m[1][1]/det, -m[0][1]/det,
//             -m[1][0]/det,  m[0][0]/det
//             };
//         }

//         PWEMat2 operator*(const PWEMat2& other) const {
//             PWEMat2 res;
//             for (int i = 0; i < 2; i++) {
//                 for (int j = 0; j < 2; j++) {
//                     res.m[i][j] =
//                         m[i][0] * other.m[0][j] +
//                         m[i][1] * other.m[1][j];
//                 }
//             }
//             return res;
//         }
//         PWEVec2 operator*(const PWEVec2& v) const {
//             return {
//                 m[0][0] * v.x + m[0][1] * v.y,
//                 m[1][0] * v.x + m[1][1] * v.y
//             };
//         }
//     };
//     struct PWEMat3
//     {
//         float m[3][3] = {
//             {1,0,0},
//             {0,1,0},
//             {0,0,1}
//         };

//         static PWEMat3 Identity() {
//             return PWEMat3();
//         }

//         static PWEMat3 Translate(const PWEVec2& t) {
//             return {
//                 1,0, t.x,
//                 0,1, t.y,
//                 0,0, 1
//             };
//         }

//         static PWEMat3 Rotate(float rad) {
//             float c = cos(rad);
//             float s = sin(rad);
//             return {
//                 c,-s, 0,
//                 s, c, 0,
//                 0, 0, 1
//             };
//         }

//         static PWEMat3 Scale(const PWEVec2& s) {
//             return {
//                 s.x, 0, 0,
//                 0, s.y, 0,
//                 0, 0, 1
//             };
//         }

//         static PWEMat3 Transform(const PWEVec2& pos, float rot, const PWEVec2& scale) {
//             return Translate(pos) * Rotate(rot) * Scale(scale);
//         }

//         PWEMat3 Inverse() const
//         {
//             float det = 
//                 m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
//                 m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
//                 m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

//             float invDet = 1.0f / det;

//             PWEMat3 res;
//             res.m[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
//             res.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
//             res.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
//             res.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
//             res.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
//             res.m[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invDet;
//             res.m[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
//             res.m[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invDet;
//             res.m[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;
//             return res;
//         }

//         PWEMat3 operator*(const PWEMat3& other) const {
//             PWEMat3 res;
//             for (int i = 0; i < 3; i++) {
//                 for (int j = 0; j < 3; j++) {
//                     res.m[i][j] =
//                         m[i][0] * other.m[0][j] +
//                         m[i][1] * other.m[1][j] +
//                         m[i][2] * other.m[2][j];
//                 }
//             }
//             return res;
//         }

//         PWEVec2 operator*(const PWEVec2& v) const {
//             return {
//                 m[0][0] * v.x + m[0][1] * v.y + m[0][2],
//                 m[1][0] * v.x + m[1][1] * v.y + m[1][2]
//             };
//         }
//     };
//     struct PWEMat4
//     {
//         float m[4][4] = {
//             {1,0,0,0},
//             {0,1,0,0},
//             {0,0,1,0},
//             {0,0,0,1}
//         };

//         static PWEMat4 Identity() {
//             return PWEMat4();
//         }

//         static PWEMat4 Translate(const PWEVec3& t) {
//             return {
//                 1,0,0, t.x,
//                 0,1,0, t.y,
//                 0,0,1, t.z,
//                 0,0,0, 1
//             };
//         }

//         static PWEMat4 Scale(const PWEVec3& s) {
//             return {
//                 s.x,0,0,0,
//                 0,s.y,0,0,
//                 0,0,s.z,0,
//                 0,0,0,1
//             };
//         }

//         PWEMat4 Inverse() const
//         {
//             float coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
//             float coef01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
//             float coef02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
//             float coef03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
//             float coef04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
//             float coef05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

//             float coef10 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
//             float coef11 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
//             float coef12 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
//             float coef13 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
//             float coef14 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
//             float coef15 = m[1][0] * m[3][1] - m[3][0] * m[1][1];

//             float coef20 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
//             float coef21 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
//             float coef22 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
//             float coef23 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
//             float coef24 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
//             float coef25 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

//             float det = 
//                 m[0][0] * (m[1][1] * coef00 - m[1][2] * coef01 + m[1][3] * coef02) -
//                 m[0][1] * (m[1][0] * coef00 - m[1][2] * coef03 + m[1][3] * coef04) +
//                 m[0][2] * (m[1][0] * coef01 - m[1][1] * coef03 + m[1][3] * coef05) -
//                 m[0][3] * (m[1][0] * coef02 - m[1][1] * coef04 + m[1][2] * coef05);

//             det = 1.0f / det;

//             PWEMat4 res;
//             res.m[0][0] = (m[1][1] * coef00 - m[1][2] * coef01 + m[1][3] * coef02) * det;
//             res.m[0][1] = (m[0][2] * coef01 - m[0][1] * coef00 - m[0][3] * coef02) * det;
//             res.m[0][2] = (m[0][1] * coef03 - m[0][2] * coef04 + m[0][3] * coef01) * det;
//             res.m[0][3] = (m[0][2] * coef02 - m[0][1] * coef05 - m[0][0] * coef00) * det;
//             res.m[1][0] = (m[1][2] * coef10 - m[1][1] * coef11 - m[1][3] * coef12) * det;
//             res.m[1][1] = (m[0][0] * coef11 - m[0][2] * coef10 + m[0][3] * coef12) * det;
//             res.m[1][2] = (m[0][2] * coef13 - m[0][0] * coef14 - m[0][3] * coef10) * det;
//             res.m[1][3] = (m[0][0] * coef12 - m[0][1] * coef13 - m[0][2] * coef15) * det;
//             res.m[2][0] = (m[1][1] * coef20 - m[1][2] * coef21 + m[1][3] * coef22) * det;
//             res.m[2][1] = (m[0][2] * coef21 - m[0][1] * coef20 - m[0][3] * coef22) * det;
//             res.m[2][2] = (m[0][0] * coef22 - m[0][1] * coef23 + m[0][3] * coef20) * det;
//             res.m[2][3] = (m[0][1] * coef25 - m[0][0] * coef22 - m[0][2] * coef21) * det;
//             res.m[3][0] = (m[1][2] * coef15 - m[1][0] * coef25 - m[1][1] * coef24) * det;
//             res.m[3][1] = (m[0][0] * coef24 - m[0][2] * coef15 + m[0][3] * coef21) * det;
//             res.m[3][2] = (m[1][0] * coef12 - m[0][0] * coef22 - m[0][1] * coef15) * det;
//             res.m[3][3] = (m[0][1] * coef20 - m[0][0] * coef21 - m[0][2] * coef11) * det;
//             return res;
//         }

//         PWEMat4 operator*(const PWEMat4& other) const {
//             PWEMat4 res;
//             for (int i = 0; i < 4; i++) {
//                 for (int j = 0; j < 4; j++) {
//                     res.m[i][j] =
//                         m[i][0] * other.m[0][j] +
//                         m[i][1] * other.m[1][j] +
//                         m[i][2] * other.m[2][j] +
//                         m[i][3] * other.m[3][j];
//                 }
//             }
//             return res;
//         }

//         PWEVec4 operator*(const PWEVec4& v) const {
//             return {
//                 m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w,
//                 m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w,
//                 m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w,
//                 m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w
//             };
//         }
//     };
// }