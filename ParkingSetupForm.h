#pragma once
#include <msclr/marshal_cppstd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include "ParkingSlot.h"
#include <direct.h>  // For _mkdir
#include <sys/stat.h> // For stat

namespace ConsoleApplication3 {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	public ref class ParkingSetupForm : public System::Windows::Forms::Form
	{
	public:
		ParkingSetupForm(void)
		{
			InitializeComponent();
			parkingManager = new ParkingManager();
			isDrawing = false;
			currentPolygon = new std::vector<cv::Point>();
			templateFrame = new cv::Mat();
			
			// ??????????????????? Templates ???????????
			EnsureTemplateFolderExists();
		}

	protected:
		~ParkingSetupForm()
		{
			if (components) delete components;
			if (parkingManager) delete parkingManager;
			if (currentPolygon) delete currentPolygon;
			if (templateFrame) delete templateFrame;
		}

	private:
		ParkingManager* parkingManager;
		cv::Mat* templateFrame;
		std::vector<cv::Point>* currentPolygon;
		bool isDrawing;
		System::ComponentModel::Container^ components;
		
		// ???????????? Templates
		//static const std::string TEMPLATE_FOLDER = "parking_templates";

	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::Panel^ panel2;
	private: System::Windows::Forms::Button^ btnLoadVideo;
	private: System::Windows::Forms::Button^ btnLoadImage;
	private: System::Windows::Forms::Button^ btnClearAll;
	private: System::Windows::Forms::Button^ btnDeleteLast;
	private: System::Windows::Forms::Button^ btnSaveTemplate;
	private: System::Windows::Forms::Button^ btnLoadTemplate;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ lblStatus;
	private: System::Windows::Forms::TextBox^ txtTemplateName;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::TextBox^ txtDescription;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::ListBox^ listBoxSlots;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->lblStatus = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->listBoxSlots = (gcnew System::Windows::Forms::ListBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->txtDescription = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txtTemplateName = (gcnew System::Windows::Forms::TextBox());
			this->btnLoadTemplate = (gcnew System::Windows::Forms::Button());
			this->btnSaveTemplate = (gcnew System::Windows::Forms::Button());
			this->btnDeleteLast = (gcnew System::Windows::Forms::Button());
			this->btnClearAll = (gcnew System::Windows::Forms::Button());
			this->btnLoadImage = (gcnew System::Windows::Forms::Button());
			this->btnLoadVideo = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->panel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::LightSteelBlue;
			this->panel1->Controls->Add(this->pictureBox1);
			this->panel1->Controls->Add(this->lblStatus);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Padding = System::Windows::Forms::Padding(10);
			this->panel1->Size = System::Drawing::Size(1200, 700);
			this->panel1->TabIndex = 0;
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::White;
			this->pictureBox1->Cursor = System::Windows::Forms::Cursors::Cross;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(10, 10);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(1180, 650);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &ParkingSetupForm::pictureBox1_MouseClick);
			// 
			// lblStatus
			// 
			this->lblStatus->BackColor = System::Drawing::Color::Yellow;
			this->lblStatus->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->lblStatus->Font = (gcnew System::Drawing::Font(L"Segoe UI", 11.25F, System::Drawing::FontStyle::Bold));
			this->lblStatus->Location = System::Drawing::Point(10, 660);
			this->lblStatus->Name = L"lblStatus";
			this->lblStatus->Size = System::Drawing::Size(1180, 30);
			this->lblStatus->TabIndex = 1;
			this->lblStatus->Text = L"Status: Load image or video to start";
			this->lblStatus->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::WhiteSmoke;
			this->panel2->Controls->Add(this->listBoxSlots);
			this->panel2->Controls->Add(this->label3);
			this->panel2->Controls->Add(this->txtDescription);
			this->panel2->Controls->Add(this->label2);
			this->panel2->Controls->Add(this->txtTemplateName);
			this->panel2->Controls->Add(this->btnLoadTemplate);
			this->panel2->Controls->Add(this->btnSaveTemplate);
			this->panel2->Controls->Add(this->btnDeleteLast);
			this->panel2->Controls->Add(this->btnClearAll);
			this->panel2->Controls->Add(this->btnLoadImage);
			this->panel2->Controls->Add(this->btnLoadVideo);
			this->panel2->Controls->Add(this->label1);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Right;
			this->panel2->Location = System::Drawing::Point(1200, 0);
			this->panel2->Name = L"panel2";
			this->panel2->Padding = System::Windows::Forms::Padding(10);
			this->panel2->Size = System::Drawing::Size(300, 700);
			this->panel2->TabIndex = 1;
			// 
			// listBoxSlots
			// 
			this->listBoxSlots->Font = (gcnew System::Drawing::Font(L"Consolas", 9.75F));
			this->listBoxSlots->FormattingEnabled = true;
			this->listBoxSlots->ItemHeight = 15;
			this->listBoxSlots->Location = System::Drawing::Point(13, 430);
			this->listBoxSlots->Name = L"listBoxSlots";
			this->listBoxSlots->Size = System::Drawing::Size(274, 244);
			this->listBoxSlots->TabIndex = 11;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold));
			this->label3->Location = System::Drawing::Point(13, 406);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(123, 21);
			this->label3->TabIndex = 10;
			this->label3->Text = L"Parking Slots:";
			// 
			// txtDescription
			// 
			this->txtDescription->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F));
			this->txtDescription->Location = System::Drawing::Point(13, 305);
			this->txtDescription->Multiline = true;
			this->txtDescription->Name = L"txtDescription";
			this->txtDescription->Size = System::Drawing::Size(274, 60);
			this->txtDescription->TabIndex = 9;
			this->txtDescription->Text = L"Parking area for ...";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Bold));
			this->label2->Location = System::Drawing::Point(13, 285);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(86, 17);
			this->label2->TabIndex = 8;
			this->label2->Text = L"Description:";
			// 
			// txtTemplateName
			// 
			this->txtTemplateName->Font = (gcnew System::Drawing::Font(L"Segoe UI", 11.25F));
			this->txtTemplateName->Location = System::Drawing::Point(13, 255);
			this->txtTemplateName->Name = L"txtTemplateName";
			this->txtTemplateName->Size = System::Drawing::Size(274, 27);
			this->txtTemplateName->TabIndex = 7;
			this->txtTemplateName->Text = L"ParkingTemplate_1";
			// 
			// btnLoadTemplate
			// 
			this->btnLoadTemplate->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(108)), 
				static_cast<System::Int32>(static_cast<System::Byte>(117)), static_cast<System::Int32>(static_cast<System::Byte>(125)));
			this->btnLoadTemplate->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnLoadTemplate->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.75F, System::Drawing::FontStyle::Bold));
			this->btnLoadTemplate->ForeColor = System::Drawing::Color::White;
			this->btnLoadTemplate->Location = System::Drawing::Point(157, 192);
			this->btnLoadTemplate->Name = L"btnLoadTemplate";
			this->btnLoadTemplate->Size = System::Drawing::Size(130, 40);
			this->btnLoadTemplate->TabIndex = 6;
			this->btnLoadTemplate->Text = L"?? Load Template";
			this->btnLoadTemplate->UseVisualStyleBackColor = false;
			this->btnLoadTemplate->Click += gcnew System::EventHandler(this, &ParkingSetupForm::btnLoadTemplate_Click);
			// 
			// btnSaveTemplate
			// 
			this->btnSaveTemplate->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), 
				static_cast<System::Int32>(static_cast<System::Byte>(167)), static_cast<System::Int32>(static_cast<System::Byte>(69)));
			this->btnSaveTemplate->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnSaveTemplate->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.75F, System::Drawing::FontStyle::Bold));
			this->btnSaveTemplate->ForeColor = System::Drawing::Color::White;
			this->btnSaveTemplate->Location = System::Drawing::Point(13, 192);
			this->btnSaveTemplate->Name = L"btnSaveTemplate";
			this->btnSaveTemplate->Size = System::Drawing::Size(130, 40);
			this->btnSaveTemplate->TabIndex = 5;
			this->btnSaveTemplate->Text = L"?? Save Template";
			this->btnSaveTemplate->UseVisualStyleBackColor = false;
			this->btnSaveTemplate->Click += gcnew System::EventHandler(this, &ParkingSetupForm::btnSaveTemplate_Click);
			// 
			// btnDeleteLast
			// 
			this->btnDeleteLast->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(193)), static_cast<System::Int32>(static_cast<System::Byte>(7)));
			this->btnDeleteLast->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnDeleteLast->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.75F, System::Drawing::FontStyle::Bold));
			this->btnDeleteLast->ForeColor = System::Drawing::Color::Black;
			this->btnDeleteLast->Location = System::Drawing::Point(157, 146);
			this->btnDeleteLast->Name = L"btnDeleteLast";
			this->btnDeleteLast->Size = System::Drawing::Size(130, 40);
			this->btnDeleteLast->TabIndex = 4;
			this->btnDeleteLast->Text = L"? Delete Last";
			this->btnDeleteLast->UseVisualStyleBackColor = false;
			this->btnDeleteLast->Click += gcnew System::EventHandler(this, &ParkingSetupForm::btnDeleteLast_Click);
			// 
			// btnClearAll
			// 
			this->btnClearAll->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(220)), 
				static_cast<System::Int32>(static_cast<System::Byte>(53)), static_cast<System::Int32>(static_cast<System::Byte>(69)));
			this->btnClearAll->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnClearAll->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.75F, System::Drawing::FontStyle::Bold));
			this->btnClearAll->ForeColor = System::Drawing::Color::White;
			this->btnClearAll->Location = System::Drawing::Point(13, 146);
			this->btnClearAll->Name = L"btnClearAll";
			this->btnClearAll->Size = System::Drawing::Size(130, 40);
			this->btnClearAll->TabIndex = 3;
			this->btnClearAll->Text = L"??? Clear All";
			this->btnClearAll->UseVisualStyleBackColor = false;
			this->btnClearAll->Click += gcnew System::EventHandler(this, &ParkingSetupForm::btnClearAll_Click);
			// 
			// btnLoadImage
			// 
			this->btnLoadImage->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(123)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->btnLoadImage->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnLoadImage->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.75F, System::Drawing::FontStyle::Bold));
			this->btnLoadImage->ForeColor = System::Drawing::Color::White;
			this->btnLoadImage->Location = System::Drawing::Point(157, 60);
			this->btnLoadImage->Name = L"btnLoadImage";
			this->btnLoadImage->Size = System::Drawing::Size(130, 80);
			this->btnLoadImage->TabIndex = 2;
			this->btnLoadImage->Text = L"??? Load Image";
			this->btnLoadImage->UseVisualStyleBackColor = false;
			this->btnLoadImage->Click += gcnew System::EventHandler(this, &ParkingSetupForm::btnLoadImage_Click);
			// 
			// btnLoadVideo
			// 
			this->btnLoadVideo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(123)), static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->btnLoadVideo->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnLoadVideo->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.75F, System::Drawing::FontStyle::Bold));
			this->btnLoadVideo->ForeColor = System::Drawing::Color::White;
			this->btnLoadVideo->Location = System::Drawing::Point(13, 60);
			this->btnLoadVideo->Name = L"btnLoadVideo";
			this->btnLoadVideo->Size = System::Drawing::Size(130, 80);
			this->btnLoadVideo->TabIndex = 1;
			this->btnLoadVideo->Text = L"?? Load Video";
			this->btnLoadVideo->UseVisualStyleBackColor = false;
			this->btnLoadVideo->Click += gcnew System::EventHandler(this, &ParkingSetupForm::btnLoadVideo_Click);
			// 
			// label1
			// 
			this->label1->Dock = System::Windows::Forms::DockStyle::Top;
			this->label1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold));
			this->label1->Location = System::Drawing::Point(10, 10);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(280, 40);
			this->label1->TabIndex = 0;
			this->label1->Text = L"??? Parking Setup";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// ParkingSetupForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1500, 700);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->panel2);
			this->Name = L"ParkingSetupForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Parking Slot Setup - Draw Parking Areas";
			this->panel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	// *** [FIXED] Helper function ?????? Template Folder ***
	private: std::string GetTemplateFolderPath() {
		return "parking_templates";
	}

	// *** [NEW] ????????????????????? Template ***
	private: void EnsureTemplateFolderExists() {
		try {
			std::string folder = GetTemplateFolderPath();
			struct stat info;
			if (stat(folder.c_str(), &info) != 0) {
				#ifdef _WIN32
					_mkdir(folder.c_str());
				#else
					mkdir(folder.c_str(), 0755);
				#endif
				
				String^ msg = "Created template folder: " + gcnew String(folder.c_str());
				lblStatus->Text = msg;
			}
		}
		catch (...) {
			MessageBox::Show("Warning: Could not create template folder!", "Warning", 
				MessageBoxButtons::OK, MessageBoxIcon::Warning);
		}
	}

	// *** [FIXED] ????????????? Path ?????????? Template ***
	private: std::string GetTemplatePath(const std::string& filename) {
		std::string name = filename;
		if (name.find(".xml") == std::string::npos) {
			name += ".xml";
		}
		std::string folder = GetTemplateFolderPath();
		return folder + "/" + name;
	}

	private: Bitmap^ MatToBitmap(cv::Mat& mat) {
		if (mat.empty()) return nullptr;
		cv::Mat rgb;
		if (mat.channels() == 1) cv::cvtColor(mat, rgb, cv::COLOR_GRAY2BGR);
		else if (mat.channels() == 4) cv::cvtColor(mat, rgb, cv::COLOR_BGRA2BGR);
		else rgb = mat;

		int width = rgb.cols;
		int height = rgb.rows;
		Bitmap^ bmp = gcnew Bitmap(width, height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
		System::Drawing::Rectangle rect(0, 0, width, height);
		System::Drawing::Imaging::BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);

		for (int y = 0; y < height; y++) {
			memcpy((unsigned char*)bmpData->Scan0.ToPointer() + y * bmpData->Stride, rgb.data + y * rgb.step, width * 3);
		}

		bmp->UnlockBits(bmpData);
		return bmp;
	}

	private: void UpdateDisplay() {
		if (templateFrame->empty()) return;

		cv::Mat display = templateFrame->clone();

		// Draw all saved slots
		for (const auto& slot : parkingManager->getSlots()) {
			std::vector<std::vector<cv::Point>> contours = { slot.polygon };
			cv::drawContours(display, contours, 0, cv::Scalar(0, 255, 0), 2);
			cv::putText(display, std::to_string(slot.id), slot.getCenter(),
				cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 2);
		}

		// Draw current polygon being drawn
		if (currentPolygon->size() > 0) {
			for (size_t i = 0; i < currentPolygon->size(); i++) {
				cv::circle(display, (*currentPolygon)[i], 5, cv::Scalar(0, 0, 255), -1);
				if (i > 0) {
					cv::line(display, (*currentPolygon)[i - 1], (*currentPolygon)[i], cv::Scalar(255, 0, 0), 2);
				}
			}
		}

		pictureBox1->Image = MatToBitmap(display);
		UpdateSlotList();
	}

	private: void UpdateSlotList() {
		listBoxSlots->Items->Clear();
		for (const auto& slot : parkingManager->getSlots()) {
			String^ item = "Slot " + slot.id + " (" + slot.polygon.size() + " points)";
			listBoxSlots->Items->Add(item);
		}
		lblStatus->Text = "Total Slots: " + parkingManager->getSlots().size() + 
			" | Drawing: " + (isDrawing ? "YES" : "NO");
	}

	private: System::Void btnLoadVideo_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Video Files|*.mp4;*.avi;*.mkv;*.mov";
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			std::string filename = msclr::interop::marshal_as<std::string>(ofd->FileName);
			cv::VideoCapture cap(filename);
			if (cap.isOpened()) {
				cap >> *templateFrame;
				if (!templateFrame->empty()) {
					parkingManager->setTemplateFrame(*templateFrame);
					UpdateDisplay();
					lblStatus->Text = "Video loaded! Click to draw parking slots (Right-click to finish slot)";
					MessageBox::Show("Video loaded successfully!\n\nLeft-click: Add point\nRight-click: Finish current slot",
						"Ready", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
			}
		}
	}

	private: System::Void btnLoadImage_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Image Files|*.jpg;*.png;*.jpeg;*.bmp";
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			std::string filename = msclr::interop::marshal_as<std::string>(ofd->FileName);
			*templateFrame = cv::imread(filename);
			if (!templateFrame->empty()) {
				parkingManager->setTemplateFrame(*templateFrame);
				UpdateDisplay();
				lblStatus->Text = "Image loaded! Click to draw parking slots (Right-click to finish slot)";
				MessageBox::Show("Image loaded successfully!\n\nLeft-click: Add point\nRight-click: Finish current slot",
					"Ready", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
		}
	}

	private: System::Void pictureBox1_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (templateFrame->empty()) return;

		// Convert PictureBox coordinates to image coordinates
		float scaleX = (float)templateFrame->cols / pictureBox1->ClientSize.Width;
		float scaleY = (float)templateFrame->rows / pictureBox1->ClientSize.Height;
		
		int imgX = (int)(e->X * scaleX);
		int imgY = (int)(e->Y * scaleY);

		if (e->Button == System::Windows::Forms::MouseButtons::Left) {
			// Add point to current polygon
			currentPolygon->push_back(cv::Point(imgX, imgY));
			isDrawing = true;
			UpdateDisplay();
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right) {
			// Finish current polygon
			if (currentPolygon->size() >= 3) {
				parkingManager->addSlot(*currentPolygon);
				currentPolygon->clear();
				isDrawing = false;
				UpdateDisplay();
				MessageBox::Show("Parking slot added!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
			else {
				MessageBox::Show("Need at least 3 points to create a parking slot!", "Error", 
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
			}
		}
	}

	private: System::Void btnClearAll_Click(System::Object^ sender, System::EventArgs^ e) {
		if (MessageBox::Show("Clear all parking slots?", "Confirm", 
			MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
			parkingManager->clearSlots();
			currentPolygon->clear();
			isDrawing = false;
			UpdateDisplay();
		}
	}

	private: System::Void btnDeleteLast_Click(System::Object^ sender, System::EventArgs^ e) {
		auto& slots = parkingManager->getSlots();
		if (!slots.empty()) {
			slots.pop_back();
			UpdateDisplay();
		}
	}

	private: System::Void btnSaveTemplate_Click(System::Object^ sender, System::EventArgs^ e) {
		if (parkingManager->getSlots().empty()) {
			MessageBox::Show("Please draw at least one parking slot!", "Error", 
				MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return;
		}

		String^ templateName = txtTemplateName->Text->Trim();
		if (String::IsNullOrEmpty(templateName)) {
			MessageBox::Show("Please enter a template name!", "Error", 
				MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return;
		}

		std::string filename = msclr::interop::marshal_as<std::string>(templateName);
		std::string fullPath = GetTemplatePath(filename);
		
		std::string name = msclr::interop::marshal_as<std::string>(txtTemplateName->Text);
		std::string desc = msclr::interop::marshal_as<std::string>(txtDescription->Text);
		
		if (parkingManager->saveTemplate(fullPath, name, desc)) {
			String^ msg = "Template saved successfully!\n\nFile: " + gcnew String(fullPath.c_str());
			MessageBox::Show(msg, "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
			lblStatus->Text = "Saved: " + gcnew String(fullPath.c_str());
		}
		else {
			MessageBox::Show("Failed to save template!", "Error", 
				MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}

	private: System::Void btnLoadTemplate_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Parking Template|*.xml";
		
		std::string folder = GetTemplateFolderPath();
		ofd->InitialDirectory = gcnew String(folder.c_str());
		ofd->Title = "Load Parking Template";
		
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			std::string filename = msclr::interop::marshal_as<std::string>(ofd->FileName);
			
			if (parkingManager->loadTemplate(filename)) {
				UpdateDisplay();
				MessageBox::Show("Template loaded successfully!", "Success", 
					MessageBoxButtons::OK, MessageBoxIcon::Information);
				lblStatus->Text = "Loaded: " + ofd->FileName;
			}
			else {
				MessageBox::Show("Failed to load template!", "Error", 
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}
	}

	// ...existing other methods...
	};
}
