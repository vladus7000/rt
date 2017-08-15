#pragma once
#include "EditorWrap.hpp"

namespace Editor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// Summary for EditorMain
	/// </summary>
	public ref class EditorMain : public System::Windows::Forms::Form
	{
	public:
		delegate void AddMaterialToEngineDelegate(MaterialWrap^);
		delegate void ViewMaterialInEditorDelegate(MaterialWrap^);
		delegate void ViewMaterialDelegate(String^);

		ref class ThreadX
		{
			EditorWrap^ m_engine;
			ListBox^ m_listBox;
			HWND m_hwnd;
			EditorMain^ m_form;
			MaterialWrap^ materialToUpdate;
			String^ materialToView;
		public:
			AddMaterialToEngineDelegate^ m_addMaterialToEngineDelegate;
			ViewMaterialDelegate^ getMaterialFromEngineDelegate;

			ThreadX(EditorWrap^ engine, ListBox^ listBox, HWND hwnd, EditorMain^ form)
			{
				m_engine = engine;
				m_listBox = listBox;
				m_hwnd = hwnd;
				m_form = form;
				m_addMaterialToEngineDelegate = gcnew AddMaterialToEngineDelegate(this, &ThreadX::AddMaterial);
				getMaterialFromEngineDelegate = gcnew ViewMaterialDelegate(this, &ThreadX::getMaterial);
			}

			void ThreadEntryPoint()
			{
				m_engine->init(m_hwnd);

				for (int i = 0; i < m_engine->getMaterialsCount(); i++)
				{
					m_form->Invoke(m_form->myDelegate, gcnew String(m_engine->getMaterialsName(i)));
				}

				while (g_engineRunning)
				{
					m_engine->tick();

					if (materialToUpdate)
					{
						if (m_engine->updateMaterial(materialToUpdate))
						{
							m_form->Invoke(m_form->myDelegate, materialToUpdate->getName());
						}
						materialToUpdate = nullptr;
					}

					
					if (materialToView)
					{
						m_form->Invoke(m_form->m_vewMaterialInEditorDelegate, m_engine->getMaterial(materialToView));
						materialToView = nullptr;
					}

				}

				m_engine->deinit();
			}

		public:	void AddMaterial(MaterialWrap^ material)
		{
			materialToUpdate = material;
		}
		public:	void getMaterial(String^ name)
		{
			materialToView = name;
		}
		};


		static bool g_engineRunning = false;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;

	public:
		delegate void AddListItem(String^);
		delegate void ViewMaterial(MaterialWrap^);
		AddListItem^ myDelegate;
		ViewMaterial^ viewMaterialDelegate;
		AddMaterialToEngineDelegate^ materialDelegateFromForm;
		ViewMaterialInEditorDelegate^ m_vewMaterialInEditorDelegate;
		ViewMaterialDelegate^ m_getMaterialFromEngineDelegate;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label5;


	private: System::Windows::Forms::TextBox^  textBox4;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  textBox7;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::TextBox^  textBox6;
	private: System::Windows::Forms::Label^  label10;

	private: System::Windows::Forms::Button^  button8;
	private: System::Windows::Forms::Button^  button6;
	private: System::Windows::Forms::Button^  button7;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::TextBox^  textBox5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  button9;
private: System::Windows::Forms::TextBox^  textBox8;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button2;
	public:
		//	 ThreadX^ engineThread;
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
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::TabPage^  tabPage4;
	private: System::Windows::Forms::TabPage^  tabPage5;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ListBox^  listBox2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ListBox^  listBox3;
	private: System::Windows::Forms::TabControl^  tabControl2;
	private: System::Windows::Forms::TabPage^  tabPage6;
	private: System::Windows::Forms::TabPage^  tabPage7;
	private: System::Windows::Forms::TabPage^  tabPage8;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;


		Thread^ m_engineMainThread;



#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->textBox5 = (gcnew System::Windows::Forms::TextBox());
			this->textBox7 = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->textBox6 = (gcnew System::Windows::Forms::TextBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->button9 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->listBox2 = (gcnew System::Windows::Forms::ListBox());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->listBox3 = (gcnew System::Windows::Forms::ListBox());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage6 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage7 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage8 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->textBox8 = (gcnew System::Windows::Forms::TextBox());
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->tabPage4->SuspendLayout();
			this->tabControl2->SuspendLayout();
			this->tabPage6->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Gray;
			this->panel1->Location = System::Drawing::Point(15, 13);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(725, 452);
			this->panel1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(6, 226);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"start RT";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &EditorMain::button1_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Controls->Add(this->tabPage5);
			this->tabControl1->Location = System::Drawing::Point(2, 12);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(1038, 512);
			this->tabControl1->TabIndex = 2;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->textBox4);
			this->tabPage1->Controls->Add(this->label8);
			this->tabPage1->Controls->Add(this->textBox3);
			this->tabPage1->Controls->Add(this->label7);
			this->tabPage1->Controls->Add(this->textBox5);
			this->tabPage1->Controls->Add(this->textBox7);
			this->tabPage1->Controls->Add(this->label4);
			this->tabPage1->Controls->Add(this->textBox2);
			this->tabPage1->Controls->Add(this->label11);
			this->tabPage1->Controls->Add(this->label6);
			this->tabPage1->Controls->Add(this->textBox8);
			this->tabPage1->Controls->Add(this->label9);
			this->tabPage1->Controls->Add(this->textBox6);
			this->tabPage1->Controls->Add(this->label10);
			this->tabPage1->Controls->Add(this->textBox1);
			this->tabPage1->Controls->Add(this->label5);
			this->tabPage1->Controls->Add(this->button9);
			this->tabPage1->Controls->Add(this->button8);
			this->tabPage1->Controls->Add(this->button3);
			this->tabPage1->Controls->Add(this->button6);
			this->tabPage1->Controls->Add(this->button7);
			this->tabPage1->Controls->Add(this->button5);
			this->tabPage1->Controls->Add(this->button4);
			this->tabPage1->Controls->Add(this->button2);
			this->tabPage1->Controls->Add(this->label3);
			this->tabPage1->Controls->Add(this->listBox1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(1030, 486);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Materials";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// textBox4
			// 
			this->textBox4->Location = System::Drawing::Point(283, 168);
			this->textBox4->Name = L"textBox4";
			this->textBox4->Size = System::Drawing::Size(229, 20);
			this->textBox4->TabIndex = 5;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(226, 168);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(38, 13);
			this->label8->TabIndex = 4;
			this->label8->Text = L"normal";
			// 
			// textBox3
			// 
			this->textBox3->Location = System::Drawing::Point(283, 142);
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(229, 20);
			this->textBox3->TabIndex = 5;
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(226, 142);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(50, 13);
			this->label7->TabIndex = 4;
			this->label7->Text = L"glosiness";
			// 
			// textBox5
			// 
			this->textBox5->Location = System::Drawing::Point(638, 189);
			this->textBox5->Name = L"textBox5";
			this->textBox5->Size = System::Drawing::Size(229, 20);
			this->textBox5->TabIndex = 5;
			// 
			// textBox7
			// 
			this->textBox7->Location = System::Drawing::Point(638, 165);
			this->textBox7->Name = L"textBox7";
			this->textBox7->Size = System::Drawing::Size(229, 20);
			this->textBox7->TabIndex = 5;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(595, 189);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(22, 13);
			this->label4->TabIndex = 4;
			this->label4->Text = L"GS";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(283, 115);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(229, 20);
			this->textBox2->TabIndex = 5;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(595, 165);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(21, 13);
			this->label11->TabIndex = 4;
			this->label11->Text = L"PS";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(226, 115);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(39, 13);
			this->label6->TabIndex = 4;
			this->label6->Text = L"albedo";
			// 
			// textBox6
			// 
			this->textBox6->Location = System::Drawing::Point(638, 139);
			this->textBox6->Name = L"textBox6";
			this->textBox6->Size = System::Drawing::Size(229, 20);
			this->textBox6->TabIndex = 5;
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(595, 139);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(21, 13);
			this->label10->TabIndex = 4;
			this->label10->Text = L"VS";
			this->label10->Click += gcnew System::EventHandler(this, &EditorMain::label5_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(283, 44);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(229, 20);
			this->textBox1->TabIndex = 5;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(226, 44);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(33, 13);
			this->label5->TabIndex = 4;
			this->label5->Text = L"name";
			this->label5->Click += gcnew System::EventHandler(this, &EditorMain::label5_Click);
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(873, 187);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(24, 23);
			this->button9->TabIndex = 3;
			this->button9->Text = L"...";
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &EditorMain::button9_Click);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(873, 163);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(24, 23);
			this->button8->TabIndex = 3;
			this->button8->Text = L"...";
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &EditorMain::button8_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(518, 168);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(24, 23);
			this->button6->TabIndex = 3;
			this->button6->Text = L"...";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &EditorMain::button6_Click);
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(873, 137);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(24, 23);
			this->button7->TabIndex = 3;
			this->button7->Text = L"...";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &EditorMain::button7_Click);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(518, 142);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(24, 23);
			this->button5->TabIndex = 3;
			this->button5->Text = L"...";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &EditorMain::button5_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(518, 113);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(24, 23);
			this->button4->TabIndex = 3;
			this->button4->Text = L"...";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &EditorMain::button4_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(552, 39);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(120, 23);
			this->button2->TabIndex = 3;
			this->button2->Text = L"addMaterial/Update";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &EditorMain::button2_Click_1);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(18, 28);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(63, 13);
			this->label3->TabIndex = 1;
			this->label3->Text = L"Material List";
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(6, 44);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(200, 407);
			this->listBox1->TabIndex = 0;
			this->listBox1->Click += gcnew System::EventHandler(this, &EditorMain::listBox1_Click);
			this->listBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &EditorMain::listBox1_MouseDown);
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->label2);
			this->tabPage2->Controls->Add(this->listBox2);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(1030, 486);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Textures";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(18, 28);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(62, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Texture List";
			// 
			// listBox2
			// 
			this->listBox2->FormattingEnabled = true;
			this->listBox2->Location = System::Drawing::Point(6, 44);
			this->listBox2->Name = L"listBox2";
			this->listBox2->Size = System::Drawing::Size(200, 407);
			this->listBox2->TabIndex = 0;
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->label1);
			this->tabPage3->Controls->Add(this->listBox3);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(1030, 486);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Models";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(18, 28);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(55, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Model List";
			// 
			// listBox3
			// 
			this->listBox3->FormattingEnabled = true;
			this->listBox3->Location = System::Drawing::Point(6, 44);
			this->listBox3->Name = L"listBox3";
			this->listBox3->Size = System::Drawing::Size(200, 407);
			this->listBox3->TabIndex = 0;
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->tabControl2);
			this->tabPage4->Controls->Add(this->panel1);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(1030, 486);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = L"Render";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// tabControl2
			// 
			this->tabControl2->Controls->Add(this->tabPage6);
			this->tabControl2->Controls->Add(this->tabPage7);
			this->tabControl2->Controls->Add(this->tabPage8);
			this->tabControl2->Location = System::Drawing::Point(746, 13);
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(281, 281);
			this->tabControl2->TabIndex = 2;
			// 
			// tabPage6
			// 
			this->tabPage6->Controls->Add(this->button1);
			this->tabPage6->Location = System::Drawing::Point(4, 22);
			this->tabPage6->Name = L"tabPage6";
			this->tabPage6->Padding = System::Windows::Forms::Padding(3);
			this->tabPage6->Size = System::Drawing::Size(273, 255);
			this->tabPage6->TabIndex = 0;
			this->tabPage6->Text = L"Settings";
			this->tabPage6->UseVisualStyleBackColor = true;
			// 
			// tabPage7
			// 
			this->tabPage7->Location = System::Drawing::Point(4, 22);
			this->tabPage7->Name = L"tabPage7";
			this->tabPage7->Padding = System::Windows::Forms::Padding(3);
			this->tabPage7->Size = System::Drawing::Size(273, 255);
			this->tabPage7->TabIndex = 1;
			this->tabPage7->Text = L"Effects";
			this->tabPage7->UseVisualStyleBackColor = true;
			// 
			// tabPage8
			// 
			this->tabPage8->Location = System::Drawing::Point(4, 22);
			this->tabPage8->Name = L"tabPage8";
			this->tabPage8->Size = System::Drawing::Size(273, 255);
			this->tabPage8->TabIndex = 2;
			this->tabPage8->Text = L"PostProcess";
			this->tabPage8->UseVisualStyleBackColor = true;
			// 
			// tabPage5
			// 
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Size = System::Drawing::Size(1030, 486);
			this->tabPage5->TabIndex = 4;
			this->tabPage5->Text = L"Shaders";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->AddExtension = false;
			this->openFileDialog1->FileName = L"openFileDialog1";
			this->openFileDialog1->Title = L"SelectFile";
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(873, 106);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(24, 23);
			this->button3->TabIndex = 3;
			this->button3->Text = L"...";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &EditorMain::button3_Click);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(581, 108);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(51, 13);
			this->label9->TabIndex = 4;
			this->label9->Text = L"EffectFile";
			this->label9->Click += gcnew System::EventHandler(this, &EditorMain::label5_Click);
			// 
			// textBox8
			// 
			this->textBox8->Location = System::Drawing::Point(638, 108);
			this->textBox8->Name = L"textBox8";
			this->textBox8->Size = System::Drawing::Size(229, 20);
			this->textBox8->TabIndex = 5;
			// 
			// EditorMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1062, 536);
			this->Controls->Add(this->tabControl1);
			this->Name = L"EditorMain";
			this->Text = L"RT Editor";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &EditorMain::EditorMain_FormClosing);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			this->tabPage4->ResumeLayout(false);
			this->tabControl2->ResumeLayout(false);
			this->tabPage6->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

		public:	void AddListItemMethod(String^ item)
		{
			listBox1->Items->Add(item);
			listBox1->Update();
		}
		public:	void ViewMaterialMethod(MaterialWrap^ m)
		{
			if (m)
			{
				textBox1->Text = m->getName();
				textBox2->Text = m->getTexture(MaterialWrap::TextureLayers::albedo);
				textBox3->Text = m->getTexture(MaterialWrap::TextureLayers::glosiness);
				textBox4->Text = m->getTexture(MaterialWrap::TextureLayers::normal);

				textBox6->Text = m->getShader(MaterialWrap::Shader::VS);
				textBox7->Text = m->getShader(MaterialWrap::Shader::PS);
				textBox5->Text = m->getShader(MaterialWrap::Shader::GS);

				textBox8->Text = m->getEffectFile();
			}
		}

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
	{
		myDelegate = gcnew AddListItem(this, &EditorMain::AddListItemMethod);
		m_vewMaterialInEditorDelegate = gcnew ViewMaterialInEditorDelegate(this, &EditorMain::ViewMaterialMethod);

		EditorWrap^ wrap = gcnew EditorWrap();
		ThreadX^ o1 = gcnew ThreadX(wrap, listBox1, static_cast<HWND>(this->panel1->Handle.ToPointer()), this);
		m_engineMainThread = gcnew Thread(gcnew ThreadStart(o1, &ThreadX::ThreadEntryPoint));
		m_engineMainThread->Name = "engineMainThread";
		materialDelegateFromForm = o1->m_addMaterialToEngineDelegate;
		m_getMaterialFromEngineDelegate = o1->getMaterialFromEngineDelegate;
		g_engineRunning = true;

		m_engineMainThread->Start();
	}
	private: System::Void listBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
	}

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e)
	{

	}
	private: System::Void EditorMain_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
	{
		g_engineRunning = false;
	}

private: System::Void listBox1_Click(System::Object^  sender, System::EventArgs^  e) {

	
	if (m_getMaterialFromEngineDelegate/*listBox1->SelectedItem*/)
	{
		m_getMaterialFromEngineDelegate->Invoke(listBox1->SelectedItem->ToString());
		//String^ item = listBox1->SelectedItem->ToString();
		//textBox1->Text = item;
	}

}
private: System::Void button2_Click_1(System::Object^  sender, System::EventArgs^  e) {
	if (textBox1->Text != "")
	{
		MaterialWrap^ m = gcnew MaterialWrap();
		m->setName(textBox1->Text);
		m->setTexture(MaterialWrap::TextureLayers::albedo, textBox2->Text);
		m->setTexture(MaterialWrap::TextureLayers::glosiness, textBox3->Text);
		m->setTexture(MaterialWrap::TextureLayers::normal, textBox4->Text);

		m->setShader(MaterialWrap::Shader::VS, textBox6->Text);
		m->setShader(MaterialWrap::Shader::PS, textBox7->Text);
		m->setShader(MaterialWrap::Shader::GS, textBox5->Text);
		m->setEffectFile(textBox8->Text);

		if (materialDelegateFromForm)
		{
			materialDelegateFromForm->Invoke(m);
		}
	}
}
private: System::Void label5_Click(System::Object^  sender, System::EventArgs^  e) {
}
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select albedo";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox2->Text = openFileDialog1->FileName;
}
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select glosiness";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox3->Text = openFileDialog1->FileName;
}
private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select normal";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox4->Text = openFileDialog1->FileName;
}
private: System::Void button7_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select VS";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox6->Text = openFileDialog1->FileName;
}
private: System::Void button8_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select PS";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox7->Text = openFileDialog1->FileName;
}
private: System::Void button9_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select GS";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox5->Text = openFileDialog1->FileName;
}
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->Title = "Select EffectFile";
	openFileDialog1->FileName = "";
	openFileDialog1->ShowDialog();
	textBox8->Text = openFileDialog1->FileName;
}
};
}
