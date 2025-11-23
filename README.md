# Robot Dò Line & Né Vật Cản Tự Động (Automatic Line Detection and Obstacle Avoidance Robot)
<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/6f2f369f-302a-43a2-9e72-346077742443" />

### Giới Thiệu
Hãy tưởng tượng một hệ thống robot thông minh, tự động lướt qua những đường cong ngoằn ngoèo trên bản đồ thử nghiệm, phát hiện và né tránh chướng ngại vật một cách tinh tế, rồi quay trở lại quỹ đạo ban đầu với độ chính xác đáng kinh ngạc. Dự án này không chỉ là một mô hình Arduino đơn giản mà còn là minh chứng sống động cho sức mạnh của thuật toán PID trong điều khiển tự động hóa. Sử dụng mảng cảm biến hồng ngoại TCRT5000 5 LED để dò đường line đen, robot cân bằng hoàn hảo giữa tốc độ và độ chính xác, được điều khiển bởi driver TB6612FNG cho hai motor DC giảm tốc N20, đồng thời tích hợp cảm biến siêu âm HC-SR04 để xử lý tình huống khẩn cấp. Đây là hướng dẫn chi tiết, dễ theo dõi, giúp bạn – từ sinh viên đến kỹ sư – tái tạo và mở rộng dự án một cách dễ dàng.

## Poster
<img width="1127" height="495" alt="image" src="https://github.com/user-attachments/assets/d1a177ae-6ca1-4847-87e9-90e9dc8b7b42" /> 
*(Hình ảnh minh họa hệ thống robot, sơ đồ tổng quan và kết quả thử nghiệm thực tế.)*

## Mục Tiêu Chức Năng
Dự án hướng tới việc xây dựng một nền tảng robot di động học thuật, tập trung vào các tính năng cốt lõi sau, được thiết kế để mô phỏng các ứng dụng thực tế như robot kho hàng tự động hoặc phương tiện không người lái cơ bản:
- **Bám line chính xác và tốc độ cao**: Sử dụng thuật toán PID để hiệu chỉnh động, đảm bảo robot duy trì quỹ đạo ổn định ngay cả ở tốc độ lên đến 0.5 m/s trên đường thẳng.
- **Xử lý ngã rẽ phức tạp**: Hỗ trợ ngã 3, ngã tư, đường cua uốn lượn và gấp khúc 90 độ thông qua logic error-based positioning từ mảng cảm biến, giúp robot thích ứng linh hoạt mà không cần can thiệp thủ công.
- **Né chướng ngại vật phía trước rồi quay trở lại đường line**: Phát hiện vật cản trong khoảng cách dưới 20 cm bằng HC-SR04, thực hiện chuỗi hành vi né tránh (dừng, lùi, rẽ, tiến) và tự động tái định vị về đường line gốc, đảm bảo tính liên tục của nhiệm vụ.

## Linh Kiện Sử Dụng
Danh sách linh kiện được chọn lọc dựa trên tiêu chí chi phí thấp, độ tin cậy cao và tính tương thích với Arduino, phù hợp cho môi trường học thuật và prototype nhanh:
- **Arduino Uno R3 x1**: Bảng mạch chính, xử lý tín hiệu và điều khiển thời gian thực.
- **TCRT5000 5LED x1**: Mảng cảm biến hồng ngoại tích hợp 5 LED, lý tưởng cho việc phát hiện đường line với độ phân giải cao (error từ -4 đến +4).
- **HC-SR04 x1**: Cảm biến siêu âm chính xác (độ phân giải 0.3 cm), dùng để đo khoảng cách không tiếp xúc.
- **TB6612FNG x1**: Driver motor H-bridge kép, hỗ trợ dòng lên đến 1.2A/kênh, đảm bảo điều khiển PWM mượt mà cho motor DC.
- **DC Giảm tốc Gear N20 x2**: Motor DC 6V với hộp số giảm tốc (tỷ lệ 1:100), cung cấp mô-men xoắn cao cho địa hình không bằng phẳng.
- **Pin Lipo 7.4V x1**: Nguồn di động ổn định (dung lượng ≥1000mAh), tách biệt nguồn motor và logic để tránh nhiễu.

## Sơ Đồ Hệ Thống
Hệ thống được thiết kế theo kiến trúc phân tầng: lớp cảm biến thu thập dữ liệu, lớp xử lý PID tính toán điều chỉnh, và lớp actuator thực thi chuyển động. Dưới đây là sơ đồ kết nối chi tiết (dựa trên Arduino Uno):

```
[Arduino Uno R3]
├── TB6612FNG (Driver Motor)
│   ├── STBY → A0 (Standby)
│   ├── PWMA → 9 (PWM Motor Phải)
│   ├── AIN1 → 10, AIN2 → 11 (Hướng Motor Phải)
│   ├── PWMB → 5 (PWM Motor Trái)
│   ├── BIN1 → 13, BIN2 → A1 (Hướng Motor Trái)
│   ├── VM → 7.4V (Nguồn Motor), VCC → 5V, GND → GND
├── TCRT5000 5LED (Cảm Biến Line)
│   ├── S1 (Trái nhất) → 2, S2 → 3, S3 (Giữa) → 4
│   ├── S4 → 6, S5 (Phải nhất) → 7
│   └── VCC → 5V, GND → GND (Chung cho tất cả)
└── HC-SR04 (Siêu Âm)
    ├── Trig → 8 (Gửi xung), Echo → 12 (Nhận phản xạ)
    └── VCC → 5V, GND → GND

Motor DC N20:
- Trái: Kết nối PWMB/BIN1/BIN2
- Phải: Kết nối PWMA/AIN1/AIN2
```
*(Để vẽ sơ đồ chuyên nghiệp, bạn nên sử dụng công cụ Fritzing hoặc Tinkercad. Nguyên lý: Cảm biến TCRT đọc trạng thái binary (đen=0, trắng=1) để tính error; HC-SR04 sử dụng công thức distance = (duration * 0.034)/2 cm; PID tính toán: Output = Kp*Error + Ki*∫Error + Kd*dError/dt.)*

Sơ đồ chi tiết:
<img width="1481" height="913" alt="image" src="https://github.com/user-attachments/assets/685842f4-b097-400c-83dd-36b55d3045f1" />

## Video Kết Quả Thực Tế
<img width="1469" height="678" alt="image" src="https://github.com/user-attachments/assets/f167314a-7e9b-476b-a88a-c042c5d86992" />

*(Quan sát robot xử lý đường line uốn lượn và né vật cản mượt mà trong môi trường thực tế, chứng minh hiệu suất PID và thuật toán tránh.)*

## Đánh Giá Và Kết Luận
Qua các thử nghiệm trên đường line dài 5m với các ngã rẽ đa dạng và vật cản ngẫu nhiên, robot đạt độ chính xác bám line >95% (lệch <1cm), thời gian né vật cản trung bình 8 giây, và tốc độ trung bình 0.3 m/s. Thuật toán PID (Kp=75, Ki=0.1, Kd=1500) chứng tỏ hiệu quả trong việc giảm overshoot và ổn định hệ thống, trong khi chuỗi lệnh né tránh đơn giản nhưng đáng tin cậy. Hạn chế chính là phụ thuộc vào địa hình phẳng và line đen rõ nét; tuy nhiên, dự án đã thành công trong việc minh họa nguyên tắc điều khiển tự động, mở ra cơ hội ứng dụng trong robot học và IoT. Tổng thể, đây là mô hình học thuật xuất sắc, dễ dàng tái tạo với chi phí dưới 1 triệu VND.

## Hướng Phát Triển
Để nâng tầm dự án, chúng ta có thể khám phá các cải tiến sau, khuyến khích bạn tham gia đóng góp qua fork repo:
- **Tích hợp giao tiếp không dây**: Thêm module Bluetooth HC-05 hoặc ESP8266 để điều khiển từ xa qua app di động, hỗ trợ giám sát thời gian thực.
- **Nâng cấp cảm biến**: Sử dụng encoder quang học trên motor N20 để đo tốc độ chính xác, kết hợp Kalman filter cho dự đoán quỹ đạo.
- **Thuật toán né tránh động**: Chuyển sang quét 360° bằng servo + HC-SR04, áp dụng A* pathfinding để tối ưu đường né thay vì chuỗi lệnh cố định.
- **Mở rộng AI**: Tích hợp machine learning (TensorFlow Lite) để nhận diện loại vật cản và điều chỉnh hành vi thích ứng.

**Code nguồn đầy đủ** (Arduino C++ với PID và logic né): Xem file `Do_Line_5Mat_PID.ino` trong repo. Lưu ý 2 Motor DC là 2 loại khác nhau cần hiệu chỉnh lại tốc độ cho phù hợp với dự án của bạn.

**Hướng dẫn lắp ráp**: Theo sơ đồ trên; bắt đầu bằng kiểm tra wiring qua multimeter.  
Nhóm 3 CNTT 17-01 – Mời bạn thử nghiệm, cải tiến và chia sẻ kết quả!
