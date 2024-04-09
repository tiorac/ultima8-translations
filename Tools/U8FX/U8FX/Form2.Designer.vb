<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPAL
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmPAL))
        Me.pbPAL = New System.Windows.Forms.PictureBox()
        Me.btnClose = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.lbRGB = New System.Windows.Forms.Label()
        Me.pbProbe = New System.Windows.Forms.PictureBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lbPALnum = New System.Windows.Forms.Label()
        Me.btnSAVEPAL = New System.Windows.Forms.Button()
        Me.SavePALDialog = New System.Windows.Forms.SaveFileDialog()
        CType(Me.pbPAL, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pbProbe, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'pbPAL
        '
        Me.pbPAL.Location = New System.Drawing.Point(2, 2)
        Me.pbPAL.Name = "pbPAL"
        Me.pbPAL.Size = New System.Drawing.Size(257, 257)
        Me.pbPAL.TabIndex = 0
        Me.pbPAL.TabStop = False
        '
        'btnClose
        '
        Me.btnClose.Location = New System.Drawing.Point(2, 315)
        Me.btnClose.Name = "btnClose"
        Me.btnClose.Size = New System.Drawing.Size(257, 25)
        Me.btnClose.TabIndex = 1
        Me.btnClose.Text = "Close"
        Me.btnClose.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(85, 267)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(33, 13)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "RGB:"
        '
        'lbRGB
        '
        Me.lbRGB.AutoSize = True
        Me.lbRGB.Location = New System.Drawing.Point(117, 267)
        Me.lbRGB.Name = "lbRGB"
        Me.lbRGB.Size = New System.Drawing.Size(91, 13)
        Me.lbRGB.TabIndex = 3
        Me.lbRGB.Text = "( 255 , 255 , 255 )"
        '
        'pbProbe
        '
        Me.pbProbe.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.pbProbe.Location = New System.Drawing.Point(220, 263)
        Me.pbProbe.Name = "pbProbe"
        Me.pbProbe.Size = New System.Drawing.Size(24, 24)
        Me.pbProbe.TabIndex = 4
        Me.pbProbe.TabStop = False
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(13, 267)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(32, 13)
        Me.Label2.TabIndex = 5
        Me.Label2.Text = "Num:"
        '
        'lbPALnum
        '
        Me.lbPALnum.AutoSize = True
        Me.lbPALnum.Location = New System.Drawing.Point(44, 267)
        Me.lbPALnum.Name = "lbPALnum"
        Me.lbPALnum.Size = New System.Drawing.Size(25, 13)
        Me.lbPALnum.TabIndex = 6
        Me.lbPALnum.Text = "255"
        '
        'btnSAVEPAL
        '
        Me.btnSAVEPAL.Location = New System.Drawing.Point(2, 289)
        Me.btnSAVEPAL.Name = "btnSAVEPAL"
        Me.btnSAVEPAL.Size = New System.Drawing.Size(257, 25)
        Me.btnSAVEPAL.TabIndex = 7
        Me.btnSAVEPAL.Text = "Save Palette (GIMP .GPL)"
        Me.btnSAVEPAL.UseVisualStyleBackColor = True
        '
        'SavePALDialog
        '
        Me.SavePALDialog.Title = "Save Palette (GIMP .gpl)"
        '
        'frmPAL
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(261, 342)
        Me.Controls.Add(Me.btnSAVEPAL)
        Me.Controls.Add(Me.lbPALnum)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.pbProbe)
        Me.Controls.Add(Me.lbRGB)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.btnClose)
        Me.Controls.Add(Me.pbPAL)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "frmPAL"
        Me.Text = "U8 Palette"
        CType(Me.pbPAL, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pbProbe, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents pbPAL As System.Windows.Forms.PictureBox
    Friend WithEvents btnClose As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents lbRGB As System.Windows.Forms.Label
    Friend WithEvents pbProbe As System.Windows.Forms.PictureBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents lbPALnum As System.Windows.Forms.Label
    Friend WithEvents btnSAVEPAL As System.Windows.Forms.Button
    Friend WithEvents SavePALDialog As System.Windows.Forms.SaveFileDialog
End Class
