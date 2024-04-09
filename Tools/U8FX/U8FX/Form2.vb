Imports System.IO
Imports System.Text

Public Class frmPAL
    Public Function GetColor(ByRef c As Color) As Integer
        ' Let's search PAL Color Index
        Dim p As Integer
        Dim Found As Boolean
        p = 0

        While p < 256 And Not Found
            If Form1.s_RGBPAL(p).R = c.R And Form1.s_RGBPAL(p).G = c.G And Form1.s_RGBPAL(p).B = c.B Then
                Found = True
            End If

            p = p + 1
        End While

        Return p - 1
    End Function

    Public Sub SetInfoColor(ByRef pb As PictureBox, ByVal x As Long, ByVal y As Long)
        Dim b As Bitmap = New Bitmap(pb.Image)
        Dim bProbe As Bitmap = New Bitmap(pbProbe.Size.Width, pbProbe.Size.Height)
        Dim G As Graphics = Graphics.FromImage(bProbe)

        Dim c As Color

        c = b.GetPixel(x, y)
        lbRGB.Text = "( " + c.R.ToString + " , " + c.G.ToString + " , " + c.B.ToString + " )"

        Dim sBrush As SolidBrush = New SolidBrush(c)
        G.FillRectangle(sBrush, 0, 0, 40, 24)
        pbProbe.Image = bProbe.Clone

        If Form1.PALLoaded Then
            lbPALnum.Text = GetColor(b.GetPixel(x, y))
        End If

        bProbe.Dispose()
        sBrush.Dispose()
        b.Dispose()
        G.Dispose()
    End Sub

    Private Sub frmPAL_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Dim x As Long, y As Long

        Dim customColor As Color
        Dim sBrush As SolidBrush = New SolidBrush(customColor)

        Dim BMP As Bitmap = New Bitmap(pbPAL.Size.Width, pbPAL.Size.Height)
        Dim G As Graphics = Graphics.FromImage(BMP)

        Dim P As Pen = New Pen(Brushes.Black)

        ' Create Image
        For y = 0 To 16
            For x = 0 To 16

                If x < 16 And y < 16 Then
                    'Get the Palette's Fill Color
                    If y = 0 Then
                        sBrush.Color = Form1.s_RGBPAL(x + y)
                    Else
                        sBrush.Color = Form1.s_RGBPAL(x + (y * 16))
                        If (x + (y * 16) > 253) Then
                            sBrush.Color = Form1.s_RGBPAL(x + (y * 16))
                        End If
                    End If

                    ' Draw Rectangle as per Palette
                    G.FillRectangle(sBrush, x * 16, y * 16, (x + 1) * 16, (y + 1) * 16)
                End If

                ' Draw a grid to do it cool
                P.Color = Color.Black
                G.DrawRectangle(P, x * 16, y * 16, (x + 1) * 16, (y + 1) * 16)
            Next x
        Next y

        pbPAL.Image = BMP.Clone

        P.Dispose()
        sBrush.Dispose()
        BMP.Dispose()

        ' Set first PAL color, for example.
        SetInfoColor(pbPAL, 1, 1)
    End Sub

    Private Sub btnClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClose.Click
        Me.Visible = False
    End Sub

    Private Sub pbPAL_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles pbPAL.MouseDown
        SetInfoColor(pbPAL, e.X, e.Y)
    End Sub

    Private Sub pbPAL_MouseEnter(ByVal sender As Object, ByVal e As System.EventArgs) Handles pbPAL.MouseEnter
        Me.Cursor = Cursors.Cross
    End Sub

    Private Sub pbPAL_MouseLeave(ByVal sender As Object, ByVal e As System.EventArgs) Handles pbPAL.MouseLeave
        Me.Cursor = Cursors.Default
    End Sub


    Private Sub btnSAVEPAL_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSAVEPAL.Click
        ' Set Directory in txtPath TextBox.
        SavePALDialog.InitialDirectory = Form1.Path
        SavePALDialog.FileName = Form1.Path + "\U8PALNEW.GPL"

        ' Set Filter for open Ultima VIII Palette (PAL) file.
        SavePALDialog.Filter = "PAL (GIMP Format) file|*.gpl|All files (*.*)|*.*"

        ' Show Dialog.
        If (SavePALDialog.ShowDialog() = DialogResult.OK) Then
            If SavePALDialog.FileName <> "" Then
                Dim fs As FileStream = Nothing

                Try
                    fs = New FileStream(SavePALDialog.FileName, FileMode.Create)
                    Using writer As StreamWriter = New StreamWriter(fs, Encoding.ASCII)

                        ' Write header.
                        writer.Write("GIMP Palette" + vbLf)
                        writer.Write("Name: U8 Palette" + vbLf)
                        writer.Write("#" + vbLf)
                        writer.Write("# U8 Palette to GIMP Palette" + vbLf)
                        writer.Write("# Ultima VIII: Pagan GraFX Tool - by L@Zar0" + vbLf)
                        writer.Write("#" + vbLf)

                        ' Write each color
                        Dim i As Integer

                        For i = 0 To 255
                            writer.Write(Form1.s_RGBPAL(i).R.ToString.PadLeft(3, " "c) + " " + _
                                         Form1.s_RGBPAL(i).G.ToString.PadLeft(3, " "c) + " " + _
                                         Form1.s_RGBPAL(i).B.ToString.PadLeft(3, " "c) + vbLf)
                        Next
                    End Using

                    fs.Close()
                Catch
                    MsgBox("Error opening .PAL file for export.")
                End Try
            End If
        End If
    End Sub
End Class