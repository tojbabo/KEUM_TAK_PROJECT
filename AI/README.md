신경망 훈련을위한 데이터 세트는 Kaggle에서 호스팅되는 Carrier and Courville Facial Expression Dataset에서 가져 왔습니다.

# 2020-07-20
Optimizer : Momentum -> Adam  
모델 정확도 : 63% -> 65%

# 2020-03-03

모델 제작 및 face_tracking 완성  
현재 모델 정확도 - 60% 정도  
정확도를 높이기 위해 할 수 있는 것  
    - 하이퍼 파라미터 튜닝  
    - epoch 수 수정 
    
## 수정 사항
1. 얼굴 인식률 늘릴 것  
2. 화면 좌우 반전  
3. 감정 전체 목록처럼 나오게  

# 2020-06-27  
서버 통합 완료  
이모티콘 추가  
  
### model_adam  
Validation Set 추가  
Epochs ( 200 )  
Learning Rate ( 0.01 )  
Batch size ( 128 )  
Activation Function ( ReLU )  
Optimizer ( Momentum -> Adam )  
