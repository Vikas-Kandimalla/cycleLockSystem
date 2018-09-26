package in.iitg.vikas.lora;

import android.content.Intent;
import android.support.annotation.NonNull;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.firebase.firestore.DocumentReference;
import com.google.firebase.firestore.DocumentSnapshot;
import com.google.firebase.firestore.EventListener;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.FirebaseFirestoreException;
import com.google.firebase.firestore.QuerySnapshot;

import org.w3c.dom.Text;
import java.util.ArrayList;
import java.util.Map;

public class MainActivity extends AppCompatActivity {

    public final static String TAG = "LoRa";
    private FirebaseFirestore db = FirebaseFirestore.getInstance();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



        final EditText cycleText = (EditText) findViewById(R.id.cycleID);
        Button getOtp = (Button) findViewById(R.id.getOTP);
        final TextView myView = (TextView) findViewById(R.id.OTPText);

        getOtp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                cycleText.setEnabled(false);
                final String cycleId = cycleText.getText().toString();
                Log.d(TAG,"Cycle Id is " + cycleId);
                final DocumentReference docRef = db.collection("passcodes").document(cycleId);
                docRef.get().addOnSuccessListener(new OnSuccessListener<DocumentSnapshot>() {
                    @Override
                    public void onSuccess(DocumentSnapshot documentSnapshot) {

                        if (documentSnapshot.exists()) {
                            Log.d(TAG, "Received data = " + documentSnapshot.getData());
                            if (!documentSnapshot.getMetadata().isFromCache()) {

                                Map<String, Object> data = documentSnapshot.getData();
                                String key = (String) data.get("key");
                                String text = "Your OTP is " + key;
                                myView.setText(text);
                                cycleText.setText("");
                                cycleText.setEnabled(true);
                            }
                        }
                        else {
                            String text = "Error : Cycle ID does not exist.";
                            myView.setText(text);
                            cycleText.setText("");
                            cycleText.setEnabled(true);
                        }
                    }
                }).addOnFailureListener(new OnFailureListener() {
                    @Override
                    public void onFailure(@NonNull Exception e) {
                        Log.e(TAG,e.toString());
                        String text = "Error : Check your Internet Connection.";
                        myView.setText(text);
                        cycleText.setText("");
                        cycleText.setEnabled(true);
                    }
                });

            }
        });






    }
}
