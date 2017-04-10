#pragma once
#include "RTProxy.hpp"
public ref class D3DWrap
{
public:
	void  Initialize(HWND hWnd)
	{
		m_engine = new EngineProxy(hWnd);
	}
	void  Shutdown() {}
	void  ProcessFrame() {}
	void  Resize() {}

protected:
	EngineProxy* m_engine;
};

namespace Editor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for EditorMain
	/// </summary>
	public ref class EditorMain : public System::Windows::Forms::Form
	{
	public:
		EditorMain(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~EditorMain()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  panel1;
	protected:
	private: System::Windows::Forms::Button^  button1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Location = System::Drawing::Point(21, 12);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(200, 119);
			this->panel1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(250, 31);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"button1";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &EditorMain::button1_Click);
			// 
			// EditorMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(370, 261);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->panel1);
			this->Name = L"EditorMain";
			this->Text = L"EditorMain";
			this->ResumeLayout(false);

		}
#pragma endregion



	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		D3DWrap^ wrap = gcnew D3DWrap();

		wrap->Initialize(static_cast<HWND>( this->panel1->Handle.ToPointer()));
	}
	};
}
