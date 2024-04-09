<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSelShape
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
        Me.OK_Button = New System.Windows.Forms.Button()
        Me.Cancel_Button = New System.Windows.Forms.Button()
        Me.rbU8FONTS = New System.Windows.Forms.RadioButton()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.rbU8SHAPES = New System.Windows.Forms.RadioButton()
        Me.rbU8GUMPS = New System.Windows.Forms.RadioButton()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'OK_Button
        '
        Me.OK_Button.Location = New System.Drawing.Point(4, 95)
        Me.OK_Button.Name = "OK_Button"
        Me.OK_Button.Size = New System.Drawing.Size(101, 24)
        Me.OK_Button.TabIndex = 0
        Me.OK_Button.Text = "Aceptar"
        '
        'Cancel_Button
        '
        Me.Cancel_Button.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Cancel_Button.Location = New System.Drawing.Point(105, 95)
        Me.Cancel_Button.Name = "Cancel_Button"
        Me.Cancel_Button.Size = New System.Drawing.Size(101, 24)
        Me.Cancel_Button.TabIndex = 1
        Me.Cancel_Button.Text = "Cancelar"
        '
        'rbU8FONTS
        '
        Me.rbU8FONTS.AutoSize = True
        Me.rbU8FONTS.Checked = True
        Me.rbU8FONTS.Location = New System.Drawing.Point(47, 22)
        Me.rbU8FONTS.Name = "rbU8FONTS"
        Me.rbU8FONTS.Size = New System.Drawing.Size(97, 17)
        Me.rbU8FONTS.TabIndex = 1
        Me.rbU8FONTS.TabStop = True
        Me.rbU8FONTS.Text = "U8FONTS.FLX"
        Me.rbU8FONTS.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.rbU8SHAPES)
        Me.GroupBox1.Controls.Add(Me.rbU8GUMPS)
        Me.GroupBox1.Controls.Add(Me.rbU8FONTS)
        Me.GroupBox1.Location = New System.Drawing.Point(5, 6)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(200, 86)
        Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Type of Shape to Load"
        '
        'rbU8SHAPES
        '
        Me.rbU8SHAPES.AutoSize = True
        Me.rbU8SHAPES.Location = New System.Drawing.Point(47, 64)
        Me.rbU8SHAPES.Name = "rbU8SHAPES"
        Me.rbU8SHAPES.Size = New System.Drawing.Size(104, 17)
        Me.rbU8SHAPES.TabIndex = 3
        Me.rbU8SHAPES.Text = "U8SHAPES.FLX"
        Me.rbU8SHAPES.UseVisualStyleBackColor = True
        '
        'rbU8GUMPS
        '
        Me.rbU8GUMPS.AutoSize = True
        Me.rbU8GUMPS.Location = New System.Drawing.Point(47, 43)
        Me.rbU8GUMPS.Name = "rbU8GUMPS"
        Me.rbU8GUMPS.Size = New System.Drawing.Size(100, 17)
        Me.rbU8GUMPS.TabIndex = 2
        Me.rbU8GUMPS.Text = "U8GUMPS.FLX"
        Me.rbU8GUMPS.UseVisualStyleBackColor = True
        '
        'frmSelShape
        '
        Me.AcceptButton = Me.OK_Button
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.Cancel_Button
        Me.ClientSize = New System.Drawing.Size(209, 122)
        Me.Controls.Add(Me.OK_Button)
        Me.Controls.Add(Me.Cancel_Button)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmSelShape"
        Me.ShowIcon = False
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Load Shape Info"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents OK_Button As System.Windows.Forms.Button
    Friend WithEvents Cancel_Button As System.Windows.Forms.Button
    Friend WithEvents rbU8FONTS As System.Windows.Forms.RadioButton
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents rbU8SHAPES As System.Windows.Forms.RadioButton
    Friend WithEvents rbU8GUMPS As System.Windows.Forms.RadioButton

End Class
