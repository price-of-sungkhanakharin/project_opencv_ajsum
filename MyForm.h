#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "UploadForm.h"
namespace ConsoleApplication3 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ToolStripContainer^ toolStripContainer1;
	private: System::Windows::Forms::MenuStrip^ menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^ toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^ toolStripMenuItem2;
	private: System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;

		   // แก้จุดที่ 1: ประกาศตัวแปรชื่อ openFileDialog (ไม่มีเลข 1)
	private: System::Windows::Forms::OpenFileDialog^ openFileDialog;

	private: System::Windows::Forms::ToolStripSeparator^ toolStripMenuItem3;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::ToolStripMenuItem^ imageToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ drawlineToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripMenuItem4;
	private: System::Windows::Forms::ToolStripMenuItem^ drawCircleToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripMenuItem6;
	private: System::Windows::Forms::ToolStripMenuItem^ convertToHSVToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripMenuItem5;
	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::ToolStripMenuItem^ saveFileMenu;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripMenuItem7;
	private: System::Windows::Forms::ToolStripMenuItem^ saveAsFileMenu;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripMenuItem8;
	private: System::Windows::Forms::SaveFileDialog^ saveFileDialog;
	private: System::Windows::Forms::ToolStrip^ toolStrip1;
	private: System::Windows::Forms::ToolStripButton^ toolStripButton1;
	private: System::Windows::Forms::ToolStripButton^ toolStripButton2;
	private: System::Windows::Forms::ToolStripButton^ toolStripButton3;
	private: System::Windows::Forms::ToolStripMenuItem^ uploadToolStripMenuItem;

	private:
		Bitmap^ bmp;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->saveFileMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem7 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->saveAsFileMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem8 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->imageToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->drawlineToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->drawCircleToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem6 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->convertToHSVToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem5 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton2 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton3 = (gcnew System::Windows::Forms::ToolStripButton());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->uploadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripContainer1->ContentPanel->SuspendLayout();
			this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			this->toolStripContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->menuStrip1->SuspendLayout();
			this->toolStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// toolStripContainer1
			// 
			// 
			// toolStripContainer1.ContentPanel
			// 
			this->toolStripContainer1->ContentPanel->Controls->Add(this->pictureBox1);
			this->toolStripContainer1->ContentPanel->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(1924, 904);
			this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
			this->toolStripContainer1->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->toolStripContainer1->Name = L"toolStripContainer1";
			this->toolStripContainer1->Size = System::Drawing::Size(1924, 959);
			this->toolStripContainer1->TabIndex = 0;
			this->toolStripContainer1->Text = L"toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->menuStrip1);
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->toolStrip1);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(1924, 904);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			// 
			// menuStrip1
			// 
			this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->toolStripMenuItem1,
					this->imageToolStripMenuItem, this->uploadToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 27);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1924, 28);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {
				this->toolStripMenuItem2,
					this->toolStripMenuItem3, this->saveFileMenu, this->toolStripMenuItem7, this->saveAsFileMenu, this->toolStripMenuItem8, this->exitToolStripMenuItem
			});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(46, 24);
			this->toolStripMenuItem1->Text = L"&File";
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripMenuItem2.Image")));
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(224, 26);
			this->toolStripMenuItem2->Text = L"&Open";
			this->toolStripMenuItem2->Click += gcnew System::EventHandler(this, &MyForm::toolStripMenuItem2_Click);
			// 
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(221, 6);
			// 
			// saveFileMenu
			// 
			this->saveFileMenu->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"saveFileMenu.Image")));
			this->saveFileMenu->Name = L"saveFileMenu";
			this->saveFileMenu->Size = System::Drawing::Size(224, 26);
			this->saveFileMenu->Text = L"&save";
			this->saveFileMenu->Click += gcnew System::EventHandler(this, &MyForm::saveFileMenu_Click);
			// 
			// toolStripMenuItem7
			// 
			this->toolStripMenuItem7->Name = L"toolStripMenuItem7";
			this->toolStripMenuItem7->Size = System::Drawing::Size(221, 6);
			// 
			// saveAsFileMenu
			// 
			this->saveAsFileMenu->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"saveAsFileMenu.Image")));
			this->saveAsFileMenu->Name = L"saveAsFileMenu";
			this->saveAsFileMenu->Size = System::Drawing::Size(224, 26);
			this->saveAsFileMenu->Text = L"&Save &As";
			this->saveAsFileMenu->Click += gcnew System::EventHandler(this, &MyForm::saveAsFileMenu_Click);
			// 
			// toolStripMenuItem8
			// 
			this->toolStripMenuItem8->Name = L"toolStripMenuItem8";
			this->toolStripMenuItem8->Size = System::Drawing::Size(221, 6);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(224, 26);
			this->exitToolStripMenuItem->Text = L"&Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::exitToolStripMenuItem_Click);
			// 
			// imageToolStripMenuItem
			// 
			this->imageToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
				this->drawlineToolStripMenuItem,
					this->toolStripMenuItem4, this->drawCircleToolStripMenuItem, this->toolStripMenuItem6, this->convertToHSVToolStripMenuItem, this->toolStripMenuItem5
			});
			this->imageToolStripMenuItem->Name = L"imageToolStripMenuItem";
			this->imageToolStripMenuItem->Size = System::Drawing::Size(65, 24);
			this->imageToolStripMenuItem->Text = L"&Image";
			this->imageToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::imageToolStripMenuItem_Click);
			// 
			// drawlineToolStripMenuItem
			// 
			this->drawlineToolStripMenuItem->Name = L"drawlineToolStripMenuItem";
			this->drawlineToolStripMenuItem->Size = System::Drawing::Size(224, 26);
			this->drawlineToolStripMenuItem->Text = L"Draw &line";
			this->drawlineToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::drawlineToolStripMenuItem_Click);
			// 
			// toolStripMenuItem4
			// 
			this->toolStripMenuItem4->Name = L"toolStripMenuItem4";
			this->toolStripMenuItem4->Size = System::Drawing::Size(221, 6);
			// 
			// drawCircleToolStripMenuItem
			// 
			this->drawCircleToolStripMenuItem->Name = L"drawCircleToolStripMenuItem";
			this->drawCircleToolStripMenuItem->Size = System::Drawing::Size(224, 26);
			this->drawCircleToolStripMenuItem->Text = L"draw &Circle";
			this->drawCircleToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::drawCircleToolStripMenuItem_Click);
			// 
			// toolStripMenuItem6
			// 
			this->toolStripMenuItem6->Name = L"toolStripMenuItem6";
			this->toolStripMenuItem6->Size = System::Drawing::Size(221, 6);
			// 
			// convertToHSVToolStripMenuItem
			// 
			this->convertToHSVToolStripMenuItem->Name = L"convertToHSVToolStripMenuItem";
			this->convertToHSVToolStripMenuItem->Size = System::Drawing::Size(224, 26);
			this->convertToHSVToolStripMenuItem->Text = L"Convert to HSV";
			this->convertToHSVToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::convertToHSVToolStripMenuItem_Click);
			// 
			// toolStripMenuItem5
			// 
			this->toolStripMenuItem5->Name = L"toolStripMenuItem5";
			this->toolStripMenuItem5->Size = System::Drawing::Size(221, 6);
			// 
			// toolStrip1
			// 
			this->toolStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->toolStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->toolStripButton1,
					this->toolStripButton2, this->toolStripButton3
			});
			this->toolStrip1->Location = System::Drawing::Point(4, 0);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(100, 27);
			this->toolStrip1->TabIndex = 1;
			// 
			// toolStripButton1
			// 
			this->toolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton1.Image")));
			this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton1->Name = L"toolStripButton1";
			this->toolStripButton1->Size = System::Drawing::Size(29, 24);
			this->toolStripButton1->Text = L"toolStripButton1";
			this->toolStripButton1->Click += gcnew System::EventHandler(this, &MyForm::toolStripButton1_Click);
			// 
			// toolStripButton2
			// 
			this->toolStripButton2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton2.Image")));
			this->toolStripButton2->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton2->Name = L"toolStripButton2";
			this->toolStripButton2->Size = System::Drawing::Size(29, 24);
			this->toolStripButton2->Text = L"toolStripButton2";
			this->toolStripButton2->Click += gcnew System::EventHandler(this, &MyForm::toolStripButton2_Click);
			// 
			// toolStripButton3
			// 
			this->toolStripButton3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton3.Image")));
			this->toolStripButton3->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton3->Name = L"toolStripButton3";
			this->toolStripButton3->Size = System::Drawing::Size(29, 24);
			this->toolStripButton3->Text = L"toolStripButton3";
			this->toolStripButton3->Click += gcnew System::EventHandler(this, &MyForm::toolStripButton3_Click);
			// 
			// openFileDialog
			// 
			this->openFileDialog->FileName = L"openFileDialog";
			this->openFileDialog->Filter = L"Image files|*.jpg;*.png";
			// 
			// saveFileDialog
			// 
			this->saveFileDialog->DefaultExt = L"png";
			this->saveFileDialog->Filter = L"Image files | *.jpg; *.png";
			// 
			// uploadToolStripMenuItem
			// 
			this->uploadToolStripMenuItem->Name = L"uploadToolStripMenuItem";
			this->uploadToolStripMenuItem->Size = System::Drawing::Size(70, 24);
			this->uploadToolStripMenuItem->Text = L"upload";
			this->uploadToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::uploadToolStripMenuItem_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1924, 959);
			this->Controls->Add(this->toolStripContainer1);
			this->MainMenuStrip = this->menuStrip1;
			this->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->Name = L"MyForm";
			this->Text = L"My Image Viewer";
			this->toolStripContainer1->ContentPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->PerformLayout();
			this->toolStripContainer1->ResumeLayout(false);
			this->toolStripContainer1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

		// --- ส่วนของการทำงาน (Event Handlers) ---

	private: System::Void exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
		Close();
	}

	private: System::Void toolStripMenuItem2_Click(System::Object^ sender, System::EventArgs^ e) {
		// แก้จุดที่ 4: เรียกใช้ openFileDialog (ไม่มีเลข 1)
		if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {

			// แก้จุดที่ 5: เรียกใช้ openFileDialog->FileName (ไม่มีเลข 1)
			Bitmap^ image = gcnew Bitmap(openFileDialog->FileName);

			bmp = gcnew Bitmap(image->Size.Width, image->Size.Height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
			bmp->SetResolution(image->HorizontalResolution, image->VerticalResolution);

			Graphics^ g = Graphics::FromImage(bmp);
			g->DrawImage(image, 0, 0);

			delete image;
			pictureBox1->Image = bmp;
		}
	}

	private: System::Void toolStripMenuItem1_Click(System::Object^ sender, System::EventArgs^ e) {}
	private: System::Void openFileDialog1_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) {}
	private: System::Void toolStripContainer1_ContentPanel_Load(System::Object^ sender, System::EventArgs^ e) {}
	private: System::Void pictureBox1_Click(System::Object^ sender, System::EventArgs^ e) {}
	private: System::Void imageToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {}

	private: System::Void drawlineToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
		using namespace cv;
		if (bmp == nullptr) return; // เพิ่มกัน Error กรณีไม่มีรูป
		System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
		System::Drawing::Imaging::BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, bmp->PixelFormat);
		Mat image(bmp->Height, bmp->Width, CV_8UC3, bmpData->Scan0.ToPointer(), bmpData->Stride);

		line(image, cv::Point(0, 0), cv::Point(bmp->Width, bmp->Height), Scalar(0, 0, 255), 5);

		bmp->UnlockBits(bmpData);
		pictureBox1->Image = bmp;
	}

	private: System::Void drawCircleToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
		using namespace cv;
		if (bmp == nullptr) return;
		System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
		System::Drawing::Imaging::BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, bmp->PixelFormat);
		Mat image(bmp->Height, bmp->Width, CV_8UC3, bmpData->Scan0.ToPointer(), bmpData->Stride);

		circle(image, cv::Point(50, 50), 20, CV_RGB(255, 0, 0), 3);

		bmp->UnlockBits(bmpData);
		pictureBox1->Image = bmp;
	}

	private: System::Void convertToHSVToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
		using namespace cv;
		if (bmp == nullptr) return;
		System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
		System::Drawing::Imaging::BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, bmp->PixelFormat);
		Mat image(bmp->Height, bmp->Width, CV_8UC3, bmpData->Scan0.ToPointer(), bmpData->Stride);

		cvtColor(image, image, COLOR_BGR2HSV);

		bmp->UnlockBits(bmpData);
		pictureBox1->Image = bmp;
	}

	private: System::Void saveFileMenu_Click(System::Object^ sender, System::EventArgs^ e) {
		if (bmp != nullptr) {
			// แก้จุดที่ 6: เรียกใช้ openFileDialog->FileName (ไม่มีเลข 1)
			bmp->Save(openFileDialog->FileName);
		}
	}

	private: System::Void saveAsFileMenu_Click(System::Object^ sender, System::EventArgs^ e) {
		if ((bmp != nullptr) && (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK))
		{
			bmp->Save(saveFileDialog->FileName);
		}
	}
	private: System::Void toolStripButton1_Click(System::Object^ sender, System::EventArgs^ e) {
		// แก้จุดที่ 4: เรียกใช้ openFileDialog (ไม่มีเลข 1)
		if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {

			// แก้จุดที่ 5: เรียกใช้ openFileDialog->FileName (ไม่มีเลข 1)
			Bitmap^ image = gcnew Bitmap(openFileDialog->FileName);

			bmp = gcnew Bitmap(image->Size.Width, image->Size.Height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
			bmp->SetResolution(image->HorizontalResolution, image->VerticalResolution);

			Graphics^ g = Graphics::FromImage(bmp);
			g->DrawImage(image, 0, 0);

			delete image;
			pictureBox1->Image = bmp;
		}

	}
private: System::Void toolStripButton2_Click(System::Object^ sender, System::EventArgs^ e) {
	if (bmp != nullptr) {
		// แก้จุดที่ 6: เรียกใช้ openFileDialog->FileName (ไม่มีเลข 1)
		bmp->Save(openFileDialog->FileName);
	}
}
private: System::Void toolStripButton3_Click(System::Object^ sender, System::EventArgs^ e) {
	if ((bmp != nullptr) && (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK))
	{
		bmp->Save(saveFileDialog->FileName);
	}
}
private: System::Void uploadToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
	// สร้าง Object ของหน้าต่างใหม่
	UploadForm^ form = gcnew UploadForm();

	// สั่งให้แสดงผล
	form->Show();
}
};
}