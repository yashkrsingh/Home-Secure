package com.manav.app2;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class LockScreen extends AppCompatActivity {
    private EditText pinEditText;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lock_screen);

        pinEditText = (EditText)findViewById(R.id.editTextPin);
        button=(Button)findViewById(R.id.loginButton);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String s = pinEditText.getText().toString();
                Log.d("pass", s);

                if (s.equalsIgnoreCase(Constants.passwd))
                    startActivity(new Intent(LockScreen.this, MainActivity.class));
            }
        });
    }
}
