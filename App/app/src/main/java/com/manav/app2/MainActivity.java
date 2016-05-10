package com.manav.app2;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Date;
import java.util.StringTokenizer;
import java.util.Timer;
import java.util.TimerTask;

import javax.net.ssl.HttpsURLConnection;

public class MainActivity extends AppCompatActivity {

    TextView temp;
    TextView humidity;
    TextView gas;
    String response;
    ToggleButton security;
    int securityEnabled=0;
    String last_f=null,last_i=null;
    private String[] values;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        temp = (TextView) findViewById(R.id.textViewTemp);
        humidity = (TextView) findViewById(R.id.textViewHumidity);
        gas = (TextView) findViewById(R.id.textViewGas);
        security = (ToggleButton)findViewById(R.id.toggleButtonSecurity);
        values = new String[5];

        Timer timer = new Timer();
        TimerTask scheduleAsyncTask = new TimerTask() {
            @Override
            public void run() {
                FetchDataTask task = new FetchDataTask();
                task.execute();
            }
        };
        timer.scheduleAtFixedRate(scheduleAsyncTask,0,2000);

        security.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked)
                    securityEnabled=1;
                else
                    securityEnabled=0;
            }
        });
    }

    class FetchDataTask extends AsyncTask<Void,Void,Void>{

        URL url;
        private HttpURLConnection connection;
        private InputStream inputStream;
        private OutputStream outputStream;
        private BufferedReader input;
        private BufferedWriter output;

        @Override
        protected Void doInBackground(Void... params) {

            try {

                url = new URL(Constants.PROTOCOL+Constants.HOST+":"+Constants.PORT+"/security="+securityEnabled);
                connection = (HttpURLConnection)url.openConnection();

                inputStream = connection.getInputStream();
                outputStream = connection.getOutputStream();
                input = new BufferedReader(new InputStreamReader(inputStream));
                output = new BufferedWriter(new OutputStreamWriter(outputStream));

                Log.d("Response", input.readLine());
                input.close();

            } catch (Exception e) {
                response=e.getMessage();
            }

            return null;
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            super.onPostExecute(aVoid);
            Log.d("MASTER YODA",response);
            StringTokenizer tokenizer = new StringTokenizer(response);
            tokenizer.nextToken("&|?");
            try {   for (int i = 0; i < values.length; i++) {
                values[i]= tokenizer.nextToken().split("=")[1];
            }
                temp.setText(values[0]);
                humidity.setText(values[1]);
                gas.setText(values[2]);

                if(values[3].equalsIgnoreCase("1"))
                {
                    AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                    builder.setIcon(R.drawable.fire);
                    builder.setTitle("Fire Alert");
                    builder.setNeutralButton("Dismiss", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    });
                    builder.show();
                }
                else if(values[4].equalsIgnoreCase("1"))
                {
                    AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                    builder.setIcon(R.drawable.alert);
                    builder.setTitle("Intrusion Alert");
                    builder.setNeutralButton("Dismiss", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    });
                    builder.show();
                }
            }catch (Exception e)
            {
                //e.printStackTrace();
            }

            }

    }
}
