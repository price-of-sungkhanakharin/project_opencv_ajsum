#pragma once
#include <msclr/marshal_cppstd.h>
#include <string>
#include <vector>
#include "BYTETracker.h"

#pragma managed(push, off)
#define NOMINMAX
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <mutex>

// --- Global Variables ---
static cv::dnn::Net* g_net = nullptr;
static std::vector<std::string> g_classes;
static std::vector<cv::Scalar> g_colors;
static cv::VideoCapture* g_cap = nullptr;
static BYTETracker* g_tracker = nullptr;

// --- Frame Sync Management (ส่วนที่เพิ่มใหม่) ---
static cv::Mat g_latestRawFrame;
static long long g_currentFrameSeq = 0; // เลขบัตรคิวของเฟรมปัจจุบัน
static std::mutex g_frameMutex;

// --- AI Status ---
static std::mutex g_processMutex;
static bool g_modelReady = false;

// --- Detection Results (with Tracking) ---
static std::vector<TrackedObject> g_trackedObjects;
static long long g_detectionSourceSeq = -1; // บอกว่ากล่องนี้มาจากเฟรมเลขที่เท่าไหร่
static std::mutex g_detectionMutex;

// --- Settings ---
static const int YOLO_INPUT_SIZE = 640;
static const float CONF_THRESHOLD = 0.25f;
static const float NMS_THRESHOLD = 0.45f;

// --- Helper Functions ---

static cv::Mat FormatToLetterbox(const cv::Mat& source, int width, int height, float& ratio, int& dw, int& dh) {
	if (source.empty()) return cv::Mat();

	float r = (std::min)((float)width / source.cols, (float)height / source.rows);
	int new_unpad_w = (int)round(source.cols * r);
	int new_unpad_h = (int)round(source.rows * r);

	dw = (width - new_unpad_w) / 2;
	dh = (height - new_unpad_h) / 2;

	cv::Mat resized;
	if (source.cols != new_unpad_w || source.rows != new_unpad_h) {
		cv::resize(source, resized, cv::Size(new_unpad_w, new_unpad_h));
	}
	else {
		resized = source.clone();
	}

	cv::Mat result(height, width, CV_8UC3, cv::Scalar(114, 114, 114));
	resized.copyTo(result(cv::Rect(dw, dh, new_unpad_w, new_unpad_h)));
	ratio = r;
	return result;
}

static void OpenGlobalCamera(int cameraIndex = 0) {
	std::lock_guard<std::mutex> lock(g_frameMutex);
	if (g_cap) { delete g_cap; g_cap = nullptr; }
	g_cap = new cv::VideoCapture(cameraIndex);
	g_currentFrameSeq = 0;
	g_detectionSourceSeq = -1;
}

static void OpenGlobalCameraFromIP(const std::string& rtspUrl) {
	std::lock_guard<std::mutex> lock(g_frameMutex);
	if (g_cap) { delete g_cap; g_cap = nullptr; }
	g_cap = new cv::VideoCapture(rtspUrl);
	g_currentFrameSeq = 0;
	g_detectionSourceSeq = -1;
}

static void InitGlobalModel(const std::string& modelPath) {
	std::lock_guard<std::mutex> lock(g_processMutex);
	g_modelReady = false;
	if (g_net) { delete g_net; g_net = nullptr; }
	if (g_tracker) { delete g_tracker; g_tracker = nullptr; }

	try {
		g_net = new cv::dnn::Net(cv::dnn::readNetFromONNX(modelPath));
		g_net->setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
		g_net->setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
		
		// Initialize tracker with better parameters
		// maxFramesLost = 90 frames (~3 seconds at 30fps)
		// iouThreshold = 0.25 (more lenient for partial occlusion)
		g_tracker = new BYTETracker(90, 0.25f);
		
		OutputDebugStringA("[INFO] Online mode with improved ByteTrack (90 frames tolerance)\n");

		g_classes = {
			"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
			"fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
			"elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
			"skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
			"tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
			"sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
			"potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
			"microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
			"hair drier", "toothbrush"
		};

		g_colors.clear();
		for (size_t i = 0; i < g_classes.size(); i++) {
			g_colors.push_back(cv::Scalar(rand() % 255, rand() % 255, rand() % 255));
		}
		g_modelReady = true;
	}
	catch (...) {
		if (g_tracker) { delete g_tracker; g_tracker = nullptr; }
	}
}

// [FIX] เพิ่ม Parameter frameSeq เพื่อระบุตัวตนของเฟรม
static void DetectObjectsOnFrame(const cv::Mat& inputFrame, long long frameSeq) {
	{
		std::lock_guard<std::mutex> lock(g_processMutex);
		if (inputFrame.empty() || !g_net || !g_modelReady || !g_tracker) return;
	}

	try {
		cv::Mat workingImage = inputFrame.clone();
		float ratio; int dw, dh;
		cv::Mat input_image = FormatToLetterbox(workingImage, YOLO_INPUT_SIZE, YOLO_INPUT_SIZE, ratio, dw, dh);
		if (input_image.empty()) return;

		cv::Mat blob;
		cv::dnn::blobFromImage(input_image, blob, 1.0 / 255.0, cv::Size(YOLO_INPUT_SIZE, YOLO_INPUT_SIZE), cv::Scalar(), true, false);

		std::vector<cv::Mat> outputs;
		{
			std::lock_guard<std::mutex> lock(g_processMutex);
			g_net->setInput(blob);
			g_net->forward(outputs, g_net->getUnconnectedOutLayersNames());
		}

		if (outputs.empty() || outputs[0].empty()) return;

		cv::Mat output_data = outputs[0];
		int rows = output_data.size[1];
		int dimensions = output_data.size[2];

		if (output_data.dims == 3) {
			output_data = output_data.reshape(1, rows);
			cv::transpose(output_data, output_data);
			rows = output_data.rows;
			dimensions = output_data.cols;
		}
		else {
			cv::Mat output_t;
			cv::transpose(output_data.reshape(1, output_data.size[1]), output_t);
			output_data = output_t;
			rows = output_data.rows;
			dimensions = output_data.cols;
		}

		float* data = (float*)output_data.data;
		std::vector<int> class_ids;
		std::vector<float> confs;
		std::vector<cv::Rect> boxes;

		for (int i = 0; i < rows; i++) {
			float* classes_scores = data + 4;
			if (dimensions >= 4 + (int)g_classes.size()) {
				cv::Mat scores(1, (int)g_classes.size(), CV_32FC1, classes_scores);
				cv::Point class_id;
				double max_class_score;
				cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

				if (max_class_score > CONF_THRESHOLD) {
					float x = data[0]; float y = data[1]; float w = data[2]; float h = data[3];
					float left = (x - 0.5 * w - dw) / ratio;
					float top = (y - 0.5 * h - dh) / ratio;
					float width = w / ratio;
					float height = h / ratio;
					boxes.push_back(cv::Rect((int)left, (int)top, (int)width, (int)height));
					confs.push_back((float)max_class_score);
					class_ids.push_back(class_id.x);
				}
			}
			data += dimensions;
		}

		std::vector<int> nms;
		cv::dnn::NMSBoxes(boxes, confs, CONF_THRESHOLD, NMS_THRESHOLD, nms);

		// Prepare detections for tracker
		std::vector<cv::Rect> nms_boxes;
		std::vector<int> nms_class_ids;
		std::vector<float> nms_confs;
		
		for (int idx : nms) {
			nms_boxes.push_back(boxes[idx]);
			nms_class_ids.push_back(class_ids[idx]);
			nms_confs.push_back(confs[idx]);
		}

		// Update tracker with new detections
		std::vector<TrackedObject> trackedObjs;
		{
			std::lock_guard<std::mutex> lock(g_processMutex);
			trackedObjs = g_tracker->update(nms_boxes, nms_class_ids, nms_confs);
		}

		// Update global tracked objects
		{
			std::lock_guard<std::mutex> detLock(g_detectionMutex);
			g_trackedObjects = trackedObjs;
			g_detectionSourceSeq = frameSeq;
		}
	}
	catch (...) {}
}

// [FIX] รับ displaySeq มาเช็ค
static cv::Mat DrawPersistentDetections(const cv::Mat& frame, long long displaySeq) {
	if (frame.empty()) return cv::Mat();
	cv::Mat result = frame.clone();

	std::lock_guard<std::mutex> lock(g_detectionMutex);

	if (g_detectionSourceSeq > displaySeq) {
		return result;
	}

	// Draw tracked objects with ID
	for (const auto& obj : g_trackedObjects) {
		if (obj.classId >= 0 && obj.classId < g_classes.size()) {
			cv::Rect box = obj.bbox;
			
			// Draw bounding box
			cv::rectangle(result, box, g_colors[obj.classId], 2);
			
			// Create compact label with ID and class name
			std::string label = "ID:" + std::to_string(obj.id) + " " + g_classes[obj.classId];
			
			int baseline;
			// Reduced font size from 0.6 to 0.4 and thickness from 2 to 1
			cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseline);
			int y_label = (std::max)(0, box.y - textSize.height - 4);
			
			// Draw smaller label background
			cv::rectangle(result, 
				cv::Point(box.x, y_label), 
				cv::Point(box.x + textSize.width, y_label + textSize.height + 4), 
				g_colors[obj.classId], -1);
			
			// Draw label text with smaller font
			cv::putText(result, label, 
				cv::Point(box.x, y_label + textSize.height + 1), 
				cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
			
			// Draw smaller tracking ID badge at bottom-right of box
			std::string idBadge = "#" + std::to_string(obj.id);
			cv::Size badgeSize = cv::getTextSize(idBadge, cv::FONT_HERSHEY_SIMPLEX, 0.35, 1, &baseline);
			cv::Point badgePos(box.x + box.width - badgeSize.width - 3, box.y + box.height - 3);
			
			// Draw smaller badge background
			cv::rectangle(result,
				cv::Point(badgePos.x - 2, badgePos.y - badgeSize.height - 2),
				cv::Point(badgePos.x + badgeSize.width + 2, badgePos.y + 2),
				cv::Scalar(0, 0, 0), -1);
			
			// Draw badge text with smaller font
			cv::putText(result, idBadge, badgePos,
				cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(255, 255, 0), 1);
		}
	}
	
	// Draw tracking statistics with smaller font
	std::string stats = "Tracks: " + std::to_string(g_trackedObjects.size());
	cv::putText(result, stats, cv::Point(10, 25),
		cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
	
	return result;
}

static void OpenGlobalVideo(const std::string& filename) {
	std::lock_guard<std::mutex> lock(g_frameMutex);
	if (g_cap) { delete g_cap; g_cap = nullptr; }
	g_cap = new cv::VideoCapture(filename);
	g_currentFrameSeq = 0; // Reset ตัวนับ
	g_detectionSourceSeq = -1;
}

static bool ReadNextVideoFrame() {
	std::lock_guard<std::mutex> lock(g_frameMutex);
	if (!g_cap || !g_cap->isOpened()) return false;
	cv::Mat frame;
	*g_cap >> frame;
	if (frame.empty()) return false;
	g_latestRawFrame = frame;
	g_currentFrameSeq++; // [FIX] เพิ่มตัวนับทุกครั้งที่มีเฟรมใหม่
	return true;
}

static void GetLatestRawFrameCopy(cv::Mat& outFrame, long long& outSeq) {
	std::lock_guard<std::mutex> lock(g_frameMutex);
	if (!g_latestRawFrame.empty()) {
		outFrame = g_latestRawFrame.clone();
		outSeq = g_currentFrameSeq;
	}
}

#pragma managed(pop)

namespace ConsoleApplication3 {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	public ref class UploadForm : public System::Windows::Forms::Form
	{
	public:
		UploadForm(void)
		{
			InitializeComponent();
			bufferLock = gcnew Object();
			currentFrame = nullptr;
			isProcessing = false;
			shouldStop = false;

			BackgroundWorker^ modelLoader = gcnew BackgroundWorker();
			modelLoader->DoWork += gcnew DoWorkEventHandler(this, &UploadForm::LoadModel_DoWork);
			modelLoader->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler(this, &UploadForm::LoadModel_Completed);
			modelLoader->RunWorkerAsync();
		}

	protected:
		~UploadForm()
		{
			StopProcessing();
			if (components) delete components;
		}

	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::Timer^ timer1;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: BackgroundWorker^ processingWorker;
	private: System::ComponentModel::IContainer^ components;
	private: Bitmap^ currentFrame;
	private: Object^ bufferLock;
	private: bool isProcessing;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::Label^ lblCameraName;
	private: System::Windows::Forms::Button^ btnPlayPause;
	private: System::Windows::Forms::TrackBar^ trackBar1;
	private: System::Windows::Forms::Button^ btnNextFrame;
	private: System::Windows::Forms::Button^ btnOnlineMode;
	private: System::Windows::Forms::Button^ btnPrevFrame;
	private: System::Windows::Forms::Panel^ panel2;
	private: System::Windows::Forms::Label^ lblViolation;
	private: System::Windows::Forms::Label^ lblNormal;
	private: System::Windows::Forms::Label^ lblEmpty;
	private: System::Windows::Forms::Button^ btnLiveCamera;
	private: System::Windows::Forms::Panel^ panel3;
	private: System::Windows::Forms::Label^ lblLogs;
	private: bool shouldStop;

#pragma region Windows Form Designer generated code
		   void InitializeComponent(void)
		   {
			   this->components = (gcnew System::ComponentModel::Container());
			   this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			   this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			   this->processingWorker = (gcnew System::ComponentModel::BackgroundWorker());
			   this->panel1 = (gcnew System::Windows::Forms::Panel());
			   this->btnPrevFrame = (gcnew System::Windows::Forms::Button());
			   this->btnNextFrame = (gcnew System::Windows::Forms::Button());
			   this->btnOnlineMode = (gcnew System::Windows::Forms::Button());
			   this->btnPlayPause = (gcnew System::Windows::Forms::Button());
			   this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			   this->lblCameraName = (gcnew System::Windows::Forms::Label());
			   this->panel2 = (gcnew System::Windows::Forms::Panel());
			   this->lblLogs = (gcnew System::Windows::Forms::Label());
			   this->panel3 = (gcnew System::Windows::Forms::Panel());
			   this->lblViolation = (gcnew System::Windows::Forms::Label());
			   this->lblNormal = (gcnew System::Windows::Forms::Label());
			   this->lblEmpty = (gcnew System::Windows::Forms::Label());
			   this->btnLiveCamera = (gcnew System::Windows::Forms::Button());
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			   this->panel1->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			   this->panel2->SuspendLayout();
			   this->SuspendLayout();
			   // 
			   // timer1
			   // 
			   this->timer1->Interval = 30;
			   this->timer1->Tick += gcnew System::EventHandler(this, &UploadForm::timer1_Tick);
			   // 
			   // pictureBox1
			   // 
			   this->pictureBox1->BackColor = System::Drawing::Color::White;
			   this->pictureBox1->Location = System::Drawing::Point(23, 80);
			   this->pictureBox1->Name = L"pictureBox1";
			   this->pictureBox1->Size = System::Drawing::Size(800, 380);
			   this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox1->TabIndex = 1;
			   this->pictureBox1->TabStop = false;
			   // 
			   // processingWorker
			   // 
			   this->processingWorker->WorkerSupportsCancellation = true;
			   this->processingWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &UploadForm::processingWorker_DoWork);
			   // 
			   // panel1
			   // 
			   this->panel1->BackColor = System::Drawing::Color::LightSteelBlue;
			   this->panel1->Controls->Add(this->btnPrevFrame);
			   this->panel1->Controls->Add(this->btnNextFrame);
			   this->panel1->Controls->Add(this->btnOnlineMode);
			   this->panel1->Controls->Add(this->btnPlayPause);
			   this->panel1->Controls->Add(this->trackBar1);
			   this->panel1->Controls->Add(this->lblCameraName);
			   this->panel1->Controls->Add(this->pictureBox1);
			   this->panel1->Location = System::Drawing::Point(12, 12);
			   this->panel1->Name = L"panel1";
			   this->panel1->Size = System::Drawing::Size(851, 484);
			   this->panel1->TabIndex = 4;
			   // 
			   // btnPrevFrame
			   // 
			   this->btnPrevFrame->BackColor = System::Drawing::Color::Yellow;
			   this->btnPrevFrame->Location = System::Drawing::Point(13, 25);
			   this->btnPrevFrame->Name = L"btnPrevFrame";
			   this->btnPrevFrame->Size = System::Drawing::Size(27, 32);
			   this->btnPrevFrame->TabIndex = 7;
			   this->btnPrevFrame->Text = L"<";
			   this->btnPrevFrame->UseVisualStyleBackColor = false;
			   // 
			   // btnNextFrame
			   // 
			   this->btnNextFrame->BackColor = System::Drawing::Color::Yellow;
			   this->btnNextFrame->Location = System::Drawing::Point(104, 24);
			   this->btnNextFrame->Name = L"btnNextFrame";
			   this->btnNextFrame->Size = System::Drawing::Size(27, 32);
			   this->btnNextFrame->TabIndex = 6;
			   this->btnNextFrame->Text = L">";
			   this->btnNextFrame->UseVisualStyleBackColor = false;
			   // 
			   // btnOnlineMode
			   // 
			   this->btnOnlineMode->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), 
		static_cast<System::Int32>(static_cast<System::Byte>(167)), static_cast<System::Int32>(static_cast<System::Byte>(69)));
	this->btnOnlineMode->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
	this->btnOnlineMode->Location = System::Drawing::Point(731, 11);
	this->btnOnlineMode->Name = L"btnOnlineMode";
	this->btnOnlineMode->Size = System::Drawing::Size(112, 46);
	this->btnOnlineMode->TabIndex = 5;
	this->btnOnlineMode->Text = L"Online";
	this->btnOnlineMode->UseVisualStyleBackColor = false;
	// 
	// btnPlayPause
	// 
	this->btnPlayPause->Location = System::Drawing::Point(470, 20);
	this->btnPlayPause->Name = L"btnPlayPause";
	this->btnPlayPause->Size = System::Drawing::Size(32, 29);
	this->btnPlayPause->TabIndex = 4;
	this->btnPlayPause->Text = L"▶";
	this->btnPlayPause->UseVisualStyleBackColor = true;
	// 
	// trackBar1
	// 
	this->trackBar1->Location = System::Drawing::Point(508, 20);
	this->trackBar1->Name = L"trackBar1";
	this->trackBar1->Size = System::Drawing::Size(217, 45);
	this->trackBar1->TabIndex = 3;
	// 
	// lblCameraName
	// 
	this->lblCameraName->AutoSize = true;
	this->lblCameraName->BackColor = System::Drawing::Color::Yellow;
	this->lblCameraName->Location = System::Drawing::Point(46, 34);
	this->lblCameraName->Name = L"lblCameraName";
	this->lblCameraName->Size = System::Drawing::Size(52, 13);
	this->lblCameraName->TabIndex = 2;
	this->lblCameraName->Text = L"Camera 1";
	// 
	// panel2
	// 
	this->panel2->BackColor = System::Drawing::Color::LightSteelBlue;
	this->panel2->Controls->Add(this->lblLogs);
	this->panel2->Controls->Add(this->panel3);
	this->panel2->Controls->Add(this->lblViolation);
	this->panel2->Controls->Add(this->lblNormal);
	this->panel2->Controls->Add(this->lblEmpty);
	this->panel2->Controls->Add(this->btnLiveCamera);
	this->panel2->Location = System::Drawing::Point(869, 12);
	this->panel2->Name = L"panel2";
	this->panel2->Size = System::Drawing::Size(541, 484);
	this->panel2->TabIndex = 5;
	// 
	// panel3
	// 
	this->panel3->BackColor = System::Drawing::Color::LemonChiffon;
	this->panel3->Location = System::Drawing::Point(116, 198);
	this->panel3->Name = L"panel3";
	this->panel3->Size = System::Drawing::Size(329, 38);
	this->panel3->TabIndex = 5;
	// 
	// lblViolation
	// 
	this->lblViolation->AutoSize = true;
	this->lblViolation->BackColor = System::Drawing::Color::OrangeRed;
	this->lblViolation->Location = System::Drawing::Point(399, 142);
	this->lblViolation->Name = L"lblViolation";
	this->lblViolation->Size = System::Drawing::Size(47, 13);
	this->lblViolation->TabIndex = 4;
	this->lblViolation->Text = L"Violation";
	// 
	// lblNormal
	// 
	this->lblNormal->AutoSize = true;
	this->lblNormal->BackColor = System::Drawing::Color::Yellow;
	this->lblNormal->Location = System::Drawing::Point(244, 142);
	this->lblNormal->Name = L"lblNormal";
	this->lblNormal->Size = System::Drawing::Size(40, 13);
	this->lblNormal->TabIndex = 3;
	this->lblNormal->Text = L"Normal";
	// 
	// lblEmpty
	// 
	this->lblEmpty->AutoSize = true;
	this->lblEmpty->BackColor = System::Drawing::Color::GreenYellow;
	this->lblEmpty->Location = System::Drawing::Point(93, 142);
	this->lblEmpty->Name = L"lblEmpty";
	this->lblEmpty->Size = System::Drawing::Size(36, 13);
	this->lblEmpty->TabIndex = 0;
	this->lblEmpty->Text = L"Empty";
	// 
	// btnLiveCamera
	// 
	this->btnLiveCamera->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), 
		static_cast<System::Int32>(static_cast<System::Byte>(167)), static_cast<System::Int32>(static_cast<System::Byte>(69)));
	this->btnLiveCamera->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
	this->btnLiveCamera->Location = System::Drawing::Point(438, 11);
	this->btnLiveCamera->Name = L"btnLiveCamera";
	this->btnLiveCamera->Size = System::Drawing::Size(100, 56);
	this->btnLiveCamera->TabIndex = 0;
	this->btnLiveCamera->Text = L"📹 Live Camera";
	this->btnLiveCamera->UseVisualStyleBackColor = false;
	this->btnLiveCamera->Click += gcnew System::EventHandler(this, &UploadForm::btnLiveCamera_Click);
	// 
	// lblLogs
	// 
	this->lblLogs->Anchor = System::Windows::Forms::AnchorStyles::Top;
	this->lblLogs->AutoSize = true;
	this->lblLogs->Font = (gcnew System::Drawing::Font(L"Segoe UI", 16.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
		static_cast<System::Byte>(0)));
	this->lblLogs->ForeColor = System::Drawing::Color::Black;
	this->lblLogs->Location = System::Drawing::Point(211, 42);
	this->lblLogs->Margin = System::Windows::Forms::Padding(100, 0, 3, 0);
	this->lblLogs->Name = L"lblLogs";
	this->lblLogs->Size = System::Drawing::Size(163, 31);
	this->lblLogs->TabIndex = 7;
	this->lblLogs->Text = L"logs 25/12/67";
	// 
	// UploadForm
	// 
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	this->ClientSize = System::Drawing::Size(1422, 508);
	this->Controls->Add(this->panel2);
	this->Controls->Add(this->panel1);
	this->Name = L"UploadForm";
	this->Text = L"Online Mode - Loading Model...";
	this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &UploadForm::UploadForm_FormClosing);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
	this->panel1->ResumeLayout(false);
	this->panel1->PerformLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
	this->panel2->ResumeLayout(false);
	this->panel2->PerformLayout();
	this->ResumeLayout(false);

		   }
#pragma endregion

	private: Bitmap^ MatToBitmap(cv::Mat& mat) {
		if (mat.empty() || mat.type() != CV_8UC3) return nullptr;
		try {
			int w = mat.cols; int h = mat.rows;
			Bitmap^ bmp = gcnew Bitmap(w, h, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
			System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, w, h);
			System::Drawing::Imaging::BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);
			for (int y = 0; y < h; y++) {
				memcpy((unsigned char*)bmpData->Scan0.ToPointer() + y * bmpData->Stride, mat.data + y * mat.step, w * 3);
			}
			bmp->UnlockBits(bmpData);
			return bmp;
		}
		catch (...) { return nullptr; }
	}

		   // [FIX] Worker Loop: ส่ง Frame Sequence ไปด้วย
	private: System::Void processingWorker_DoWork(System::Object^ sender, DoWorkEventArgs^ e) {
		BackgroundWorker^ worker = safe_cast<BackgroundWorker^>(sender);
		while (!shouldStop && !worker->CancellationPending) {
			try {
				cv::Mat frameToProcess;
				long long seq = 0;
				GetLatestRawFrameCopy(frameToProcess, seq); // เอาเลข Seq มา

				if (!frameToProcess.empty()) {
					DetectObjectsOnFrame(frameToProcess, seq); // ส่งเลข Seq ไป
				}
				else {
					Threading::Thread::Sleep(10);
				}
			}
			catch (...) { Threading::Thread::Sleep(50); }
		}
	}

		   // [FIX] Timer Loop: เช็ค Frame Sequence ก่อนวาด
	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e) {
		try {
			if (!ReadNextVideoFrame()) { StopProcessing(); return; }

			cv::Mat displayFrame;
			long long displaySeq = 0;
			GetLatestRawFrameCopy(displayFrame, displaySeq); // เอาเลข Seq ของภาพที่จะฉายมา

			if (!displayFrame.empty()) {
				// ส่ง displaySeq เข้าไปเช็ค ถ้ากล่องเป็นอนาคต (seq มากกว่า) มันจะไม่วาด
				cv::Mat result = DrawPersistentDetections(displayFrame, displaySeq);

				if (!result.empty()) {
					Bitmap^ newFrame = MatToBitmap(result);
					if (newFrame != nullptr) {
						Monitor::Enter(bufferLock);
						try {
							if (currentFrame != nullptr) delete currentFrame;
							currentFrame = newFrame;
							if (pictureBox1->Image) delete pictureBox1->Image;
							pictureBox1->Image = gcnew Bitmap(currentFrame);
						}
						finally { Monitor::Exit(bufferLock); }
					}
				}
			}
		}
		catch (...) {}
	}

	private: void StartProcessing() {
		shouldStop = false;
		isProcessing = true;
		{
			std::lock_guard<std::mutex> lock(g_detectionMutex);
			g_trackedObjects.clear();
			g_detectionSourceSeq = -1;
		}
		if (!processingWorker->IsBusy) processingWorker->RunWorkerAsync();
		timer1->Start();
	}

	private: void StopProcessing() {
		shouldStop = true;
		isProcessing = false;
		timer1->Stop();
		if (processingWorker->IsBusy) processingWorker->CancelAsync();
	}

	private: System::Void LoadModel_DoWork(System::Object^ sender, DoWorkEventArgs^ e) {
		try {
			std::string modelPath = "models/test/yolo11n.onnx";
			InitGlobalModel(modelPath);
			e->Result = true;
		}
		catch (const std::exception& ex) { e->Result = gcnew System::String(ex.what()); }
	}

	private: System::Void LoadModel_Completed(System::Object^ sender, RunWorkerCompletedEventArgs^ e) {
		if (e->Result != nullptr && e->Result->GetType() == bool::typeid && safe_cast<bool>(e->Result)) {
			this->Text = L"Online Mode - YOLO Detection (Ready)";
			btnLiveCamera->Enabled = true;
			MessageBox::Show("Model loaded!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		else {
			MessageBox::Show("Error loading model", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}

	private: System::Void btnLiveCamera_Click(System::Object^ sender, System::EventArgs^ e) {
		StopProcessing();
		
		// Create popup form for IP and Port input
		Form^ ipForm = gcnew Form();
		ipForm->Text = L"Connect to Mobile Camera";
		ipForm->Size = System::Drawing::Size(450, 320);
		ipForm->StartPosition = FormStartPosition::CenterParent;
		ipForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
		ipForm->MaximizeBox = false;
		ipForm->MinimizeBox = false;

		Label^ labelTitle = gcnew Label();
		labelTitle->Text = L"Enter Mobile Phone IP Address and Port";
		labelTitle->Location = System::Drawing::Point(20, 20);
		labelTitle->Size = System::Drawing::Size(400, 25);
		labelTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);

		Label^ labelIP = gcnew Label();
		labelIP->Text = L"IP Address:";
		labelIP->Location = System::Drawing::Point(20, 60);
		labelIP->Size = System::Drawing::Size(100, 20);

		TextBox^ textBoxIP = gcnew TextBox();
		textBoxIP->Location = System::Drawing::Point(120, 58);
		textBoxIP->Size = System::Drawing::Size(290, 25);
		textBoxIP->Text = L"192.168.1.100";

		Label^ labelPort = gcnew Label();
		labelPort->Text = L"Port:";
		labelPort->Location = System::Drawing::Point(20, 95);
		labelPort->Size = System::Drawing::Size(100, 20);

		TextBox^ textBoxPort = gcnew TextBox();
		textBoxPort->Location = System::Drawing::Point(120, 93);
		textBoxPort->Size = System::Drawing::Size(290, 25);
		textBoxPort->Text = L"8080";

		Label^ labelPath = gcnew Label();
		labelPath->Text = L"Path:";
		labelPath->Location = System::Drawing::Point(20, 130);
		labelPath->Size = System::Drawing::Size(100, 20);

		TextBox^ textBoxPath = gcnew TextBox();
		textBoxPath->Location = System::Drawing::Point(120, 128);
		textBoxPath->Size = System::Drawing::Size(290, 25);
		textBoxPath->Text = L"/video";

		Label^ labelExample = gcnew Label();
		labelExample->Text = L"Example apps: IP Webcam, DroidCam, or iVCam\nMake sure both devices are on the same WiFi network";
		labelExample->Location = System::Drawing::Point(20, 165);
		labelExample->Size = System::Drawing::Size(400, 35);
		labelExample->Font = gcnew System::Drawing::Font(L"Segoe UI", 8, FontStyle::Italic);
		labelExample->ForeColor = System::Drawing::Color::Gray;

		Button^ btnConnect = gcnew Button();
		btnConnect->Text = L"Connect";
		btnConnect->Location = System::Drawing::Point(120, 215);
		btnConnect->Size = System::Drawing::Size(100, 35);
		btnConnect->BackColor = System::Drawing::Color::FromArgb(40, 167, 69);
		btnConnect->ForeColor = System::Drawing::Color::White;
		btnConnect->FlatStyle = FlatStyle::Flat;
		btnConnect->DialogResult = System::Windows::Forms::DialogResult::OK;

		Button^ btnCancel = gcnew Button();
		btnCancel->Text = L"Cancel";
		btnCancel->Location = System::Drawing::Point(230, 215);
		btnCancel->Size = System::Drawing::Size(100, 35);
		btnCancel->BackColor = System::Drawing::Color::FromArgb(220, 53, 69);
		btnCancel->ForeColor = System::Drawing::Color::White;
		btnCancel->FlatStyle = FlatStyle::Flat;
		btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;

		ipForm->Controls->Add(labelTitle);
		ipForm->Controls->Add(labelIP);
		ipForm->Controls->Add(textBoxIP);
		ipForm->Controls->Add(labelPort);
		ipForm->Controls->Add(textBoxPort);
		ipForm->Controls->Add(labelPath);
		ipForm->Controls->Add(textBoxPath);
		ipForm->Controls->Add(labelExample);
		ipForm->Controls->Add(btnConnect);
		ipForm->Controls->Add(btnCancel);
		ipForm->AcceptButton = btnConnect;
		ipForm->CancelButton = btnCancel;

		if (ipForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			String^ ip = textBoxIP->Text->Trim();
			String^ port = textBoxPort->Text->Trim();
			String^ path = textBoxPath->Text->Trim();

			if (String::IsNullOrEmpty(ip) || String::IsNullOrEmpty(port)) {
				MessageBox::Show(
					"Please enter both IP Address and Port",
					"Input Required",
					MessageBoxButtons::OK,
					MessageBoxIcon::Warning
				);
				return;
			}

			// Ensure path starts with /
			if (!path->StartsWith("/")) {
				path = "/" + path;
			}

			try {
				// Try multiple URL formats
				array<String^>^ urlFormats = gcnew array<String^> {
					String::Format("http://{0}:{1}{2}", ip, port, path),
					String::Format("http://{0}:{1}/videofeed", ip, port),
					String::Format("http://{0}:{1}/video", ip, port),
					String::Format("rtsp://{0}:{1}", ip, port)
				};

				this->Text = L"Online Mode - Connecting to camera...";
				Application::DoEvents();

				bool connected = false;
				String^ successUrl = "";

				for each (String^ streamUrl in urlFormats) {
					std::string url = msclr::interop::marshal_as<std::string>(streamUrl);
					
					OutputDebugStringA(("[INFO] Trying to connect: " + url + "\n").c_str());
					
					OpenGlobalCameraFromIP(url);
					Threading::Thread::Sleep(1000); // Wait longer for connection

					if (g_cap && g_cap->isOpened()) {
						// Try to read a test frame
						cv::Mat testFrame;
						bool canRead = false;
						{
							std::lock_guard<std::mutex> lock(g_frameMutex);
							if (g_cap->read(testFrame)) {
								canRead = !testFrame.empty();
							}
						}

						if (canRead) {
							connected = true;
							successUrl = streamUrl;
							OutputDebugStringA("[SUCCESS] Connected successfully!\n");
							break;
						}
					}
					
					// Close failed connection
					{
						std::lock_guard<std::mutex> lock(g_frameMutex);
						if (g_cap) {
							delete g_cap;
							g_cap = nullptr;
						}
					}
				}

				if (connected) {
					StartProcessing();
					this->Text = L"Online Mode - Live Camera Connected";
					MessageBox::Show(
						"Successfully connected to mobile camera!\n\n" +
						"Stream URL: " + successUrl + "\n\n" +
						"Press OK to start detection.",
						"Connection Successful",
						MessageBoxButtons::OK,
						MessageBoxIcon::Information
					);
				}
				else {
					this->Text = L"Online Mode - Connection Failed";
					
					String^ errorMsg = "Failed to connect to mobile camera!\n\n";
					errorMsg += "Troubleshooting Steps:\n";
					errorMsg += "1. Verify IP Address: " + ip + "\n";
					errorMsg += "2. Verify Port: " + port + "\n";
					errorMsg += "3. Check if camera app is running on mobile\n";
					errorMsg += "4. Ensure both devices are on the same WiFi network\n";
					errorMsg += "5. Check firewall settings\n";
					errorMsg += "6. Try disabling antivirus temporarily\n\n";
					errorMsg += "Attempted URLs:\n";
					for each (String^ url in urlFormats) {
						errorMsg += "  - " + url + "\n";
					}
					
					MessageBox::Show(
						errorMsg,
						"Connection Error",
						MessageBoxButtons::OK,
						MessageBoxIcon::Error
					);
				}
			}
			catch (Exception^ ex) {
				this->Text = L"Online Mode - Error Occurred";
				MessageBox::Show(
					"An error occurred while connecting:\n\n" + 
					ex->Message + "\n\n" +
					"Stack Trace:\n" + ex->StackTrace,
					"Exception Error",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
			}
		}
	}

	private: System::Void UploadForm_FormClosing(System::Object^ sender, FormClosingEventArgs^ e) {
		StopProcessing();
	}
	};
}