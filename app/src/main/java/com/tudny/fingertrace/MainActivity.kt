package com.tudny.fingertrace

import android.graphics.Bitmap
import android.os.AsyncTask
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.FrameLayout
import android.widget.ProgressBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.scale
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    val tag = MainActivity::class.java.simpleName

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val myCanvas = findViewById<MyCanvas>(R.id.my_canvas)
        val clearButton = findViewById<Button>(R.id.clear_button)
        val checkButton = findViewById<Button>(R.id.check_button)
        val frameLayout = findViewById<FrameLayout>(R.id.frame_layout)

        frameLayout.layoutParams.height = frameLayout.layoutParams.width

        clearButton.setOnClickListener {
            myCanvas.clearScreen()

            val scoreText = findViewById<TextView>(R.id.score_text)
            val textPixels = findViewById<TextView>(R.id.pixels_view)
            scoreText.text = ""
            textPixels.text = ""

        }

        checkButton.setOnClickListener {
            val bitmap = myCanvas.getAsBitmap()
            val newBitmap = scaleBitmap(bitmap)
            printBitmap(newBitmap)
            val imageProcess = ImageProcess(resources.getStringArray(R.array.coefficients), this)
            imageProcess.execute(newBitmap)
        }
    }

    private fun printBitmap(bitmap: Bitmap) {
        var text = ""
        for( y in 0 until bitmap.height ) {
            var line = ""
            for (x in 0 until bitmap.width) {
                val pixel = ImageProcess.getPixelBrightness(bitmap.getPixel(x, y))
                line += (if(pixel > 100) "#  " else ".   ")
            }
            line += "\n"
            text += line
        }

        val textPixels = findViewById<TextView>(R.id.pixels_view)
        textPixels.text = text
    }

    private val scaledX = 28
    private val scaledY = 28
    private fun scaleBitmap(bitmap: Bitmap): Bitmap = bitmap.scale(scaledX, scaledY)
}

class ImageProcess(private val coefficientsString: Array<String>, private val activity: MainActivity) : AsyncTask<Bitmap, Float, Int>() {

    private val coefficients: MutableList<Float> = mutableListOf()
    private val digits = 10;

    override fun onPreExecute() {
        coefficientsString.forEach {
            coefficients.add(it.replace("a", "").toFloat())
        }
    }

    override fun doInBackground(vararg params: Bitmap?): Int {
        val bitmap = params[0] ?: return -1
        val sums = mutableListOf(0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f)
        for( digit in 0 until digits ) {
            for (y in 0 until bitmap.height) {
                for (x in 0 until bitmap.width) {
                    val pixelId = x + 28 * y;
                    val coefficientsId = digit * (28 * 28) + pixelId
                    val pixelBrightness = getPixelBrightness(bitmap.getPixel(x, y))
                    // print("$pixelBrightness ")
                    sums[digit] += pixelBrightness * coefficients[coefficientsId]
                    publishProgress((coefficientsId.toFloat() / (digits * bitmap.height * bitmap.width).toFloat()))
                }
            }
        }

        var maximalDigit = 0
        for( digit in 1 until digits ) {
            // println("Sum $digit ${sums[digit]}")
            if(sums[digit] > sums[maximalDigit]){
                maximalDigit = digit
            }
        }

        return maximalDigit
    }

    // (red * 0.2126f + green * 0.7152f + blue * 0.0722f) <- brightness
    companion object {
        fun getPixelBrightness(pixel: Int): Int {
            val red = (pixel shr 16) and 0xff
            val green = (pixel shr 8) and 0xff
            val blue = (pixel shr 0) and 0xff
            return ((red * 0.2126f + green * 0.7152f + blue * 0.0722f).toInt())
        }
    }

    override fun onProgressUpdate(vararg values: Float?) {
        val progress = values[0]?.times(100f)
        val progressBar = activity.findViewById<ProgressBar>(R.id.progress_bar)
        progressBar.progress = progress?.toInt() ?: 0
    }

    override fun onPostExecute(result: Int?) {
        val scoreText = activity.findViewById<TextView>(R.id.score_text)
        scoreText.text = result.toString()
    }
}
