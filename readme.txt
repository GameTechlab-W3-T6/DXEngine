문제 요약
- 증상: instanced draw 시 Input Assembler(IA)로 인스턴스 데이터가 안 들어온 것처럼 보임. 실제로는 IA 바인딩과 InputLayout은 정상인데, 인스턴스 버퍼에 담는 변환 데이터가 잘못 포장되어 셰이더에서 올바르게 해석되지 않아 결과가 나오지 않는 문제.

핵심 원인
- 코드 경로: `Engine/UTextholderComp.h` 의 `Build3x4Rows(...)`와 `Engine/ShaderW0VS.hlsl` 의 `main_instanced(...)` 매칭 불일치.
- 프로젝트 전반이 row-vector(행 벡터) 기반 수학을 사용합니다. 이 경우 모델 행렬의 평행이동(translation)은 “마지막 행(Row 3)”에 있어야 합니다. `Engine/Matrix.h` 의 `TransformPointRow` 구현을 보면 실제로 M[3][0..2]가 translation로 쓰이고 있습니다.
- 그런데 인스턴스 데이터 포장 단계(`Build3x4Rows`)에서는 각 행의 w 성분에 `M[0][3], M[1][3], M[2][3]`(마지막 열, Column 3)을 넣고 있습니다.
- 반면 인스턴스 VS(`ShaderW0VS.hlsl`)에서는 아래와 같이 마지막 행을 `float4(inst.M0.w, inst.M1.w, inst.M2.w, 1)`로 재구성합니다. 즉 “마지막 행(Row 3)”을 C++ 쪽에서 채운 w 성분들로 만들고 있는데, 그 w 성분들에 들어간 값이 ‘마지막 열(Column 3)’이어서 평행이동이 뒤바뀐(전치된) 형태가 됩니다.
- 결과적으로 모델 변환의 translation이 올바르게 전달되지 않아 화면상에서 기대한 위치로 인스턴스가 배치되지 않습니다. 사용자는 이를 “IA에 값이 안 간다”로 인지할 수 있습니다.

문제가 되는 부분 상세
- C++(인스턴스 포장): `Engine/UTextholderComp.h`
  - 현재 구현
    - `outM0[3] = M.M[0][3]; outM1[3] = M.M[1][3]; outM2[3] = M.M[2][3];`
    - 즉 ‘마지막 열(Column 3)’ 값을 각 행의 w에 저장
- HLSL(인스턴스 해석): `Engine/ShaderW0VS.hlsl`
  - 현재 구현
    - `row3 = float4(inst.M0.w, inst.M1.w, inst.M2.w, 1);`
    - 즉 ‘마지막 행(Row 3)’을 C++이 넣어준 w 값으로 구성
- 행 벡터(row-vector)에서는 translation이 `M.M[3][0..2]`(마지막 행)에 있어야 합니다. 지금은 `M.M[0..2][3]`(마지막 열)을 넘기고 있으므로 불일치가 발생합니다.

해결 방법(둘 중 하나 선택)
1) C++ 쪽을 고쳐서 행 벡터 규약에 맞게 translation을 넣기
   - `Build3x4Rows`를 아래처럼 수정:
     - `outM0[0..2] = M.M[0][0..2]` (그대로)
     - `outM1[0..2] = M.M[1][0..2]` (그대로)
     - `outM2[0..2] = M.M[2][0..2]` (그대로)
     - `outM0[3] = M.M[3][0];`
     - `outM1[3] = M.M[3][1];`
     - `outM2[3] = M.M[3][2];`
   - 이렇게 하면 VS의
     `float4x4 M = float4x4(float4(inst.M0.xyz, 0), float4(inst.M1.xyz, 0), float4(inst.M2.xyz, 0), float4(inst.M0.w, inst.M1.w, inst.M2.w, 1));`
     와 정확히 일치하여 올바른 translation이 적용됩니다.

2) 또는 셰이더를 컬럼 벡터 규약처럼 해석하도록 바꾸기
   - 만약 C++의 현재 포장을 유지하려면 VS에서 마지막 행을 구성하는 대신, ‘마지막 열’을 쓰는 형태(곱셈 순서 포함)를 전반적으로 일치시켜야 합니다. 다만 프로젝트 전체가 row-vector 가정이라 C++ 한 줄 수정(방법 1)이 안전하고 일관성이 좋습니다.

IA/바인딩 자체는 정상
- `IASetVertexBuffers(0, 2, ...)`로 슬롯 0에 정점 버퍼, 슬롯 1에 인스턴스 버퍼가 바인딩되고, 인스턴스 요소들의 `InputSlotClass=D3D11_INPUT_PER_INSTANCE_DATA`, `InstanceDataStepRate=1`도 정상입니다.
- `InputLayoutTextInst`의 요소 오프셋(0,16,32, 48,56,64)과 `FTextInstance`의 메모리 배치(3x float4 + float2 + float2 + float4 = 80 bytes)도 일치합니다.
- 따라서 “IA에 값이 안 간다”가 아니라 “IA로 간 값이 수학적 규약 불일치로 VS에서 잘못 해석됨”이 정확한 원인입니다.

검증 방법
- 수정 전: 동일한 메시에 대해 인스턴스 Translation을 바꿔도 화면에서 제 위치에 배치되지 않거나 원점 근처에 몰립니다.
- 수정 후: `penX` 증가에 따라 문자(또는 인스턴스)가 좌우로 올바르게 나열되고, 빌보딩 행렬 적용 시 카메라를 향해 정상적으로 보입니다.
- 필요 시 첫 인스턴스의 `inst.color`를 특이 색으로 주고, VS에서 `output.Color = inst.Color;`로 확인하면 인스턴스 데이터 스트림 자체는 정상임을 시각적으로 확인할 수 있습니다.

참고(부수 사항)
- `Engine/UMesh.cpp`에서 `vbd.ByteWidth`의 삼항 연산 양쪽이 모두 `sizeof(FVertexPosUV4)`로 되어 있어(주석/변수명과 혼동 가능), 컬러 전용 정점 사용 시 혼란을 줄 수 있습니다. 본 이슈와 직접 연관은 없지만 나중에 유지보수 시 주의가 필요합니다.

정리
- 문제는 IA 바인딩/레이아웃이 아니라 “행 벡터 수학 규약 대비 인스턴스 행렬의 translation 포장 방식”입니다.
- `Build3x4Rows`에서 translation을 `M.M[3][0..2]`로 넣도록 고치면 인스턴스 데이터가 VS에서 기대대로 해석되어 정상 렌더링됩니다.
