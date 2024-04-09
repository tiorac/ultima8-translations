<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form reemplaza a Dispose para limpiar la lista de componentes.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Requerido por el Diseñador de Windows Forms
    Private components As System.ComponentModel.IContainer

    'NOTA: el Diseñador de Windows Forms necesita el siguiente procedimiento
    'Se puede modificar usando el Diseñador de Windows Forms.  
    'No lo modifique con el editor de código.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Form1))
        Me.btnOpen = New System.Windows.Forms.Button()
        Me.OpenPALDialog = New System.Windows.Forms.OpenFileDialog()
        Me.txtPath = New System.Windows.Forms.TextBox()
        Me.btnExit = New System.Windows.Forms.Button()
        Me.txtPAL = New System.Windows.Forms.TextBox()
        Me.btnPAL = New System.Windows.Forms.Button()
        Me.OpenFLXDialog = New System.Windows.Forms.OpenFileDialog()
        Me.nudShape = New System.Windows.Forms.NumericUpDown()
        Me.nudFrame = New System.Windows.Forms.NumericUpDown()
        Me.lbShapes = New System.Windows.Forms.Label()
        Me.lbFrames = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.lbSO = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.cbCompressed = New System.Windows.Forms.CheckBox()
        Me.lbSZ = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.lbFS = New System.Windows.Forms.Label()
        Me.lbFOS = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.lbGS = New System.Windows.Forms.Label()
        Me.lbCF = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.cbAZ = New System.Windows.Forms.CheckBox()
        Me.cbTransparency = New System.Windows.Forms.CheckBox()
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.tbZoom = New System.Windows.Forms.TrackBar()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.gbFI = New System.Windows.Forms.GroupBox()
        Me.pbFrame_invisible = New System.Windows.Forms.PictureBox()
        Me.pbFrame = New System.Windows.Forms.PictureBox()
        Me.pbFrameBorder = New System.Windows.Forms.PictureBox()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.btnLS = New System.Windows.Forms.Button()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.btnEAF = New System.Windows.Forms.Button()
        Me.btnSS = New System.Windows.Forms.Button()
        Me.btnIF = New System.Windows.Forms.Button()
        Me.btnEF = New System.Windows.Forms.Button()
        Me.ExpFRMDialog = New System.Windows.Forms.SaveFileDialog()
        Me.ImpFRMDialog = New System.Windows.Forms.OpenFileDialog()
        Me.SaveSHPDialog = New System.Windows.Forms.SaveFileDialog()
        Me.ExpSFDFDialog = New System.Windows.Forms.FolderBrowserDialog()
        Me.LoadSHPDialog = New System.Windows.Forms.OpenFileDialog()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.lbShapeType = New System.Windows.Forms.Label()
        CType(Me.nudShape, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudFrame, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        CType(Me.tbZoom, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox3.SuspendLayout()
        Me.gbFI.SuspendLayout()
        CType(Me.pbFrame_invisible, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pbFrame, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pbFrameBorder, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox5.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnOpen
        '
        Me.btnOpen.Location = New System.Drawing.Point(557, 6)
        Me.btnOpen.Name = "btnOpen"
        Me.btnOpen.Size = New System.Drawing.Size(100, 24)
        Me.btnOpen.TabIndex = 0
        Me.btnOpen.Text = "Open (.FLX)"
        Me.btnOpen.UseVisualStyleBackColor = True
        '
        'OpenPALDialog
        '
        Me.OpenPALDialog.FileName = "U8PAL.PAL"
        Me.OpenPALDialog.Title = "Open U8 Palette file"
        '
        'txtPath
        '
        Me.txtPath.Location = New System.Drawing.Point(5, 8)
        Me.txtPath.Name = "txtPath"
        Me.txtPath.ReadOnly = True
        Me.txtPath.Size = New System.Drawing.Size(546, 20)
        Me.txtPath.TabIndex = 1
        '
        'btnExit
        '
        Me.btnExit.Location = New System.Drawing.Point(663, 510)
        Me.btnExit.Name = "btnExit"
        Me.btnExit.Size = New System.Drawing.Size(200, 29)
        Me.btnExit.TabIndex = 4
        Me.btnExit.Text = "Exit"
        Me.btnExit.UseVisualStyleBackColor = True
        '
        'txtPAL
        '
        Me.txtPAL.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.txtPAL.Location = New System.Drawing.Point(63, 45)
        Me.txtPAL.Name = "txtPAL"
        Me.txtPAL.ReadOnly = True
        Me.txtPAL.Size = New System.Drawing.Size(64, 13)
        Me.txtPAL.TabIndex = 6
        '
        'btnPAL
        '
        Me.btnPAL.Location = New System.Drawing.Point(9, 16)
        Me.btnPAL.Name = "btnPAL"
        Me.btnPAL.Size = New System.Drawing.Size(182, 24)
        Me.btnPAL.TabIndex = 7
        Me.btnPAL.Text = "Show Palette"
        Me.btnPAL.UseVisualStyleBackColor = True
        '
        'OpenFLXDialog
        '
        Me.OpenFLXDialog.Title = "Open FLEX File"
        '
        'nudShape
        '
        Me.nudShape.Location = New System.Drawing.Point(89, 21)
        Me.nudShape.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudShape.Name = "nudShape"
        Me.nudShape.Size = New System.Drawing.Size(60, 20)
        Me.nudShape.TabIndex = 9
        Me.nudShape.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudShape.UpDownAlign = System.Windows.Forms.LeftRightAlignment.Left
        Me.nudShape.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'nudFrame
        '
        Me.nudFrame.Location = New System.Drawing.Point(89, 45)
        Me.nudFrame.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudFrame.Name = "nudFrame"
        Me.nudFrame.Size = New System.Drawing.Size(60, 20)
        Me.nudFrame.TabIndex = 10
        Me.nudFrame.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudFrame.UpDownAlign = System.Windows.Forms.LeftRightAlignment.Left
        Me.nudFrame.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'lbShapes
        '
        Me.lbShapes.AutoSize = True
        Me.lbShapes.Location = New System.Drawing.Point(152, 23)
        Me.lbShapes.Name = "lbShapes"
        Me.lbShapes.Size = New System.Drawing.Size(21, 13)
        Me.lbShapes.TabIndex = 13
        Me.lbShapes.Text = "/ 1"
        '
        'lbFrames
        '
        Me.lbFrames.AutoSize = True
        Me.lbFrames.Location = New System.Drawing.Point(153, 47)
        Me.lbFrames.Name = "lbFrames"
        Me.lbFrames.Size = New System.Drawing.Size(21, 13)
        Me.lbFrames.TabIndex = 14
        Me.lbFrames.Text = "/ 1"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(4, 23)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(81, 13)
        Me.Label1.TabIndex = 15
        Me.Label1.Text = "Shape Number:"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(5, 47)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(79, 13)
        Me.Label2.TabIndex = 16
        Me.Label2.Text = "Frame Number:"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.lbShapeType)
        Me.GroupBox1.Controls.Add(Me.Label12)
        Me.GroupBox1.Controls.Add(Me.lbSO)
        Me.GroupBox1.Controls.Add(Me.Label10)
        Me.GroupBox1.Controls.Add(Me.cbCompressed)
        Me.GroupBox1.Controls.Add(Me.lbSZ)
        Me.GroupBox1.Controls.Add(Me.Label9)
        Me.GroupBox1.Controls.Add(Me.lbFS)
        Me.GroupBox1.Controls.Add(Me.lbFOS)
        Me.GroupBox1.Controls.Add(Me.Label7)
        Me.GroupBox1.Controls.Add(Me.Label6)
        Me.GroupBox1.Controls.Add(Me.lbGS)
        Me.GroupBox1.Controls.Add(Me.lbCF)
        Me.GroupBox1.Controls.Add(Me.Label5)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.nudShape)
        Me.GroupBox1.Controls.Add(Me.nudFrame)
        Me.GroupBox1.Controls.Add(Me.lbFrames)
        Me.GroupBox1.Controls.Add(Me.lbShapes)
        Me.GroupBox1.Location = New System.Drawing.Point(663, 6)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(200, 201)
        Me.GroupBox1.TabIndex = 17
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "General Info"
        '
        'lbSO
        '
        Me.lbSO.AutoSize = True
        Me.lbSO.Location = New System.Drawing.Point(113, 145)
        Me.lbSO.Name = "lbSO"
        Me.lbSO.Size = New System.Drawing.Size(13, 13)
        Me.lbSO.TabIndex = 29
        Me.lbSO.Text = "0"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Location = New System.Drawing.Point(43, 145)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(72, 13)
        Me.Label10.TabIndex = 28
        Me.Label10.Text = "Shape Offset:"
        '
        'cbCompressed
        '
        Me.cbCompressed.AutoSize = True
        Me.cbCompressed.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.cbCompressed.Enabled = False
        Me.cbCompressed.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.cbCompressed.Location = New System.Drawing.Point(118, 179)
        Me.cbCompressed.Name = "cbCompressed"
        Me.cbCompressed.Size = New System.Drawing.Size(78, 18)
        Me.cbCompressed.TabIndex = 25
        Me.cbCompressed.Text = "Compress"
        Me.cbCompressed.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.cbCompressed.UseVisualStyleBackColor = True
        '
        'lbSZ
        '
        Me.lbSZ.AutoSize = True
        Me.lbSZ.Location = New System.Drawing.Point(113, 164)
        Me.lbSZ.Name = "lbSZ"
        Me.lbSZ.Size = New System.Drawing.Size(13, 13)
        Me.lbSZ.TabIndex = 27
        Me.lbSZ.Text = "0"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(51, 162)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(64, 13)
        Me.Label9.TabIndex = 26
        Me.Label9.Text = "Shape Size:"
        '
        'lbFS
        '
        Me.lbFS.AutoSize = True
        Me.lbFS.Location = New System.Drawing.Point(113, 107)
        Me.lbFS.Name = "lbFS"
        Me.lbFS.Size = New System.Drawing.Size(13, 13)
        Me.lbFS.TabIndex = 24
        Me.lbFS.Text = "0"
        '
        'lbFOS
        '
        Me.lbFOS.AutoSize = True
        Me.lbFOS.Location = New System.Drawing.Point(113, 90)
        Me.lbFOS.Name = "lbFOS"
        Me.lbFOS.Size = New System.Drawing.Size(13, 13)
        Me.lbFOS.TabIndex = 23
        Me.lbFOS.Text = "0"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(53, 107)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(62, 13)
        Me.Label7.TabIndex = 22
        Me.Label7.Text = "Frame Size:"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(45, 90)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(70, 13)
        Me.Label6.TabIndex = 21
        Me.Label6.Text = "Frame Offset:"
        '
        'lbGS
        '
        Me.lbGS.AutoSize = True
        Me.lbGS.Location = New System.Drawing.Point(113, 127)
        Me.lbGS.Name = "lbGS"
        Me.lbGS.Size = New System.Drawing.Size(30, 13)
        Me.lbGS.TabIndex = 20
        Me.lbGS.Text = "0 x 0"
        '
        'lbCF
        '
        Me.lbCF.AutoSize = True
        Me.lbCF.Location = New System.Drawing.Point(113, 73)
        Me.lbCF.Name = "lbCF"
        Me.lbCF.Size = New System.Drawing.Size(30, 13)
        Me.lbCF.TabIndex = 19
        Me.lbCF.Text = "0 x 0"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(4, 127)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(111, 13)
        Me.Label5.TabIndex = 18
        Me.Label5.Text = "Current Shape W x H:"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(6, 73)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(109, 13)
        Me.Label4.TabIndex = 17
        Me.Label4.Text = "Current Frame W x H:"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.cbAZ)
        Me.GroupBox2.Controls.Add(Me.cbTransparency)
        Me.GroupBox2.Controls.Add(Me.GroupBox4)
        Me.GroupBox2.Location = New System.Drawing.Point(663, 218)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(200, 92)
        Me.GroupBox2.TabIndex = 18
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "Graphical Options"
        '
        'cbAZ
        '
        Me.cbAZ.AutoSize = True
        Me.cbAZ.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.cbAZ.Enabled = False
        Me.cbAZ.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.cbAZ.Location = New System.Drawing.Point(-7, 72)
        Me.cbAZ.Name = "cbAZ"
        Me.cbAZ.Size = New System.Drawing.Size(79, 18)
        Me.cbAZ.TabIndex = 16
        Me.cbAZ.Text = "Autozoom"
        Me.cbAZ.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.cbAZ.UseVisualStyleBackColor = True
        '
        'cbTransparency
        '
        Me.cbTransparency.AutoSize = True
        Me.cbTransparency.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.cbTransparency.Enabled = False
        Me.cbTransparency.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.cbTransparency.Location = New System.Drawing.Point(70, 72)
        Me.cbTransparency.Name = "cbTransparency"
        Me.cbTransparency.Size = New System.Drawing.Size(122, 18)
        Me.cbTransparency.TabIndex = 15
        Me.cbTransparency.Text = "Real Transparency"
        Me.cbTransparency.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.cbTransparency.UseVisualStyleBackColor = True
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.tbZoom)
        Me.GroupBox4.Location = New System.Drawing.Point(9, 19)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(182, 48)
        Me.GroupBox4.TabIndex = 14
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "Zoom"
        '
        'tbZoom
        '
        Me.tbZoom.AutoSize = False
        Me.tbZoom.Enabled = False
        Me.tbZoom.LargeChange = 2
        Me.tbZoom.Location = New System.Drawing.Point(8, 15)
        Me.tbZoom.Minimum = 1
        Me.tbZoom.Name = "tbZoom"
        Me.tbZoom.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.tbZoom.Size = New System.Drawing.Size(167, 32)
        Me.tbZoom.TabIndex = 18
        Me.tbZoom.TickStyle = System.Windows.Forms.TickStyle.TopLeft
        Me.tbZoom.Value = 1
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(14, 44)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(49, 13)
        Me.Label3.TabIndex = 19
        Me.Label3.Text = "PAL File:"
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.btnPAL)
        Me.GroupBox3.Controls.Add(Me.Label3)
        Me.GroupBox3.Controls.Add(Me.txtPAL)
        Me.GroupBox3.Location = New System.Drawing.Point(663, 321)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(200, 62)
        Me.GroupBox3.TabIndex = 20
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "U8 Palette"
        '
        'gbFI
        '
        Me.gbFI.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None
        Me.gbFI.Controls.Add(Me.pbFrame_invisible)
        Me.gbFI.Controls.Add(Me.pbFrame)
        Me.gbFI.Controls.Add(Me.pbFrameBorder)
        Me.gbFI.Location = New System.Drawing.Point(5, 33)
        Me.gbFI.Name = "gbFI"
        Me.gbFI.Size = New System.Drawing.Size(653, 506)
        Me.gbFI.TabIndex = 21
        Me.gbFI.TabStop = False
        Me.gbFI.Text = "Frame Image"
        '
        'pbFrame_invisible
        '
        Me.pbFrame_invisible.Location = New System.Drawing.Point(7, 20)
        Me.pbFrame_invisible.Name = "pbFrame_invisible"
        Me.pbFrame_invisible.Size = New System.Drawing.Size(100, 50)
        Me.pbFrame_invisible.TabIndex = 8
        Me.pbFrame_invisible.TabStop = False
        Me.pbFrame_invisible.Visible = False
        '
        'pbFrame
        '
        Me.pbFrame.Location = New System.Drawing.Point(7, 20)
        Me.pbFrame.Name = "pbFrame"
        Me.pbFrame.Size = New System.Drawing.Size(640, 480)
        Me.pbFrame.TabIndex = 7
        Me.pbFrame.TabStop = False
        '
        'pbFrameBorder
        '
        Me.pbFrameBorder.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None
        Me.pbFrameBorder.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.pbFrameBorder.Location = New System.Drawing.Point(5, 18)
        Me.pbFrameBorder.Name = "pbFrameBorder"
        Me.pbFrameBorder.Size = New System.Drawing.Size(644, 484)
        Me.pbFrameBorder.TabIndex = 6
        Me.pbFrameBorder.TabStop = False
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.btnLS)
        Me.GroupBox5.Controls.Add(Me.Label11)
        Me.GroupBox5.Controls.Add(Me.Label8)
        Me.GroupBox5.Controls.Add(Me.btnEAF)
        Me.GroupBox5.Controls.Add(Me.btnSS)
        Me.GroupBox5.Controls.Add(Me.btnIF)
        Me.GroupBox5.Controls.Add(Me.btnEF)
        Me.GroupBox5.Location = New System.Drawing.Point(664, 394)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(199, 107)
        Me.GroupBox5.TabIndex = 23
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "File Tools"
        '
        'btnLS
        '
        Me.btnLS.Location = New System.Drawing.Point(45, 78)
        Me.btnLS.Name = "btnLS"
        Me.btnLS.Size = New System.Drawing.Size(75, 24)
        Me.btnLS.TabIndex = 29
        Me.btnLS.Text = "Load (.shp)"
        Me.btnLS.UseVisualStyleBackColor = True
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(6, 83)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(41, 13)
        Me.Label11.TabIndex = 28
        Me.Label11.Text = "Shape:"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(6, 21)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(39, 13)
        Me.Label8.TabIndex = 27
        Me.Label8.Text = "Frame:"
        '
        'btnEAF
        '
        Me.btnEAF.Enabled = False
        Me.btnEAF.Location = New System.Drawing.Point(4, 41)
        Me.btnEAF.Name = "btnEAF"
        Me.btnEAF.Size = New System.Drawing.Size(191, 24)
        Me.btnEAF.TabIndex = 26
        Me.btnEAF.Text = "Export ALL Frames of Shape (.png)"
        Me.btnEAF.UseVisualStyleBackColor = True
        '
        'btnSS
        '
        Me.btnSS.Enabled = False
        Me.btnSS.Location = New System.Drawing.Point(120, 78)
        Me.btnSS.Name = "btnSS"
        Me.btnSS.Size = New System.Drawing.Size(75, 24)
        Me.btnSS.TabIndex = 25
        Me.btnSS.Text = "Save (.shp)"
        Me.btnSS.UseVisualStyleBackColor = True
        '
        'btnIF
        '
        Me.btnIF.Enabled = False
        Me.btnIF.Location = New System.Drawing.Point(120, 16)
        Me.btnIF.Name = "btnIF"
        Me.btnIF.Size = New System.Drawing.Size(75, 24)
        Me.btnIF.TabIndex = 24
        Me.btnIF.Text = "Import (.png)"
        Me.btnIF.UseVisualStyleBackColor = True
        '
        'btnEF
        '
        Me.btnEF.Enabled = False
        Me.btnEF.Location = New System.Drawing.Point(45, 16)
        Me.btnEF.Name = "btnEF"
        Me.btnEF.Size = New System.Drawing.Size(75, 24)
        Me.btnEF.TabIndex = 23
        Me.btnEF.Text = "Export (.png)"
        Me.btnEF.UseVisualStyleBackColor = True
        '
        'ExpFRMDialog
        '
        Me.ExpFRMDialog.Title = "Export Frame (.png)"
        '
        'ImpFRMDialog
        '
        Me.ImpFRMDialog.Title = "Import Frame (.png)"
        '
        'SaveSHPDialog
        '
        Me.SaveSHPDialog.Title = "Save Shape (.shp)"
        '
        'LoadSHPDialog
        '
        Me.LoadSHPDialog.Title = "Load Shape (.shp)"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(3, 181)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(68, 13)
        Me.Label12.TabIndex = 20
        Me.Label12.Text = "Shape Type:"
        '
        'lbShapeType
        '
        Me.lbShapeType.AutoSize = True
        Me.lbShapeType.Location = New System.Drawing.Point(69, 181)
        Me.lbShapeType.Name = "lbShapeType"
        Me.lbShapeType.Size = New System.Drawing.Size(36, 13)
        Me.lbShapeType.TabIndex = 30
        Me.lbShapeType.Text = "FONT"
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(868, 544)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.gbFI)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.btnExit)
        Me.Controls.Add(Me.txtPath)
        Me.Controls.Add(Me.btnOpen)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.Name = "Form1"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Ultima VIII: Pagan GraFX Tool v1.0"
        CType(Me.nudShape, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudFrame, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox4.ResumeLayout(False)
        CType(Me.tbZoom, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.gbFI.ResumeLayout(False)
        CType(Me.pbFrame_invisible, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pbFrame, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pbFrameBorder, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnOpen As System.Windows.Forms.Button
    Friend WithEvents OpenPALDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents txtPath As System.Windows.Forms.TextBox
    Friend WithEvents btnExit As System.Windows.Forms.Button
    Friend WithEvents txtPAL As System.Windows.Forms.TextBox
    Friend WithEvents btnPAL As System.Windows.Forms.Button
    Friend WithEvents OpenFLXDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents nudShape As System.Windows.Forms.NumericUpDown
    Friend WithEvents nudFrame As System.Windows.Forms.NumericUpDown
    Friend WithEvents lbShapes As System.Windows.Forms.Label
    Friend WithEvents lbFrames As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents gbFI As System.Windows.Forms.GroupBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents lbGS As System.Windows.Forms.Label
    Friend WithEvents lbCF As System.Windows.Forms.Label
    Friend WithEvents pbFrameBorder As System.Windows.Forms.PictureBox
    Friend WithEvents pbFrame As System.Windows.Forms.PictureBox
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents cbTransparency As System.Windows.Forms.CheckBox
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents btnEAF As System.Windows.Forms.Button
    Friend WithEvents btnSS As System.Windows.Forms.Button
    Friend WithEvents btnIF As System.Windows.Forms.Button
    Friend WithEvents btnEF As System.Windows.Forms.Button
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents lbFS As System.Windows.Forms.Label
    Friend WithEvents lbFOS As System.Windows.Forms.Label
    Friend WithEvents cbCompressed As System.Windows.Forms.CheckBox
    Friend WithEvents lbSZ As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents lbSO As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents ExpFRMDialog As System.Windows.Forms.SaveFileDialog
    Friend WithEvents ImpFRMDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents SaveSHPDialog As System.Windows.Forms.SaveFileDialog
    Friend WithEvents tbZoom As System.Windows.Forms.TrackBar
    Friend WithEvents cbAZ As System.Windows.Forms.CheckBox
    Friend WithEvents ExpSFDFDialog As System.Windows.Forms.FolderBrowserDialog
    Friend WithEvents btnLS As System.Windows.Forms.Button
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents pbFrame_invisible As System.Windows.Forms.PictureBox
    Friend WithEvents LoadSHPDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents lbShapeType As System.Windows.Forms.Label

End Class
