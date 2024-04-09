Imports System.Windows.Forms

Public Class frmSelShape

    Private Sub OK_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OK_Button.Click
        Form1.ShapeType = 0

        If rbU8FONTS.Checked Then
            Form1.ShapeType = 1
        ElseIf rbU8GUMPS.Checked Then
            Form1.ShapeType = 2
        Else
            Form1.ShapeType = 3
        End If

        Me.Close()
    End Sub

    Private Sub Cancel_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel_Button.Click
        Form1.ShapeType = 0

        Me.Close()
    End Sub
End Class
