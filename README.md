# Assignment 1: Handwritten Digit Classification
## Gaussian and Naive Bayes Classifiers

This project implements two classifiers for handwritten digit recognition using the USPS handwritten digit dataset.

---

## Files in This Package

### 1. `assignment1_solution.py` (Main Program)
This is the main Python script that contains all the code for the assignment.

**What it does:**
- Loads the handwritten digit data from `assignment1.mat`
- Trains a Gaussian classifier with shared variance
- Trains a Naive Bayes classifier with binary features
- Evaluates both classifiers on the test set
- Generates plots and result tables
- Saves all results to files

**Classes implemented:**
- `GaussianClassifier`: Implements Gaussian classifier with independent features and shared variance
- `NaiveBayesClassifier`: Implements Naive Bayes classifier with binary features

### 2. `visualize_data.py` (Helper Script)
A utility script to visualize sample images from the dataset.

**What it does:**
- Displays sample images from both training and test sets
- Helps verify that the data is loaded correctly
- Useful for understanding the dataset structure

### 3. `README.md` (This File)
Documentation explaining what each file does and how to run the code.

---

## Requirements

### Python Version
- Python 3.6 or higher

### Required Libraries
Install the required libraries using pip:

```bash
pip install numpy scipy matplotlib
```

**Library purposes:**
- `numpy`: Numerical computations and array operations
- `scipy`: Loading MATLAB .mat files (using scipy.io)
- `matplotlib`: Plotting and visualization

---

## Dataset

### File Required
- `assignment1.mat` - MATLAB data file containing:
  - `digits_train`: Training data (shape: 64 × 700 × 10)
  - `digits_test`: Test data (shape: 64 × 400 × 10)

### Data Structure
- Each digit (0-9) has 700 training images and 400 test images
- Each image is 8×8 pixels = 64 features
- Features are in raster-scan order
- Label encoding: digits 1-9 use labels 1-9, digit '0' uses label 10 (index 9 in arrays)

---

## How to Run the Code

### Step 1: Prepare Your Environment
1. Ensure Python 3.6+ is installed
2. Install required libraries:
   ```bash
   pip install numpy scipy matplotlib
   ```

### Step 2: Place Data File
Make sure `assignment1.mat` is in the same directory as the Python scripts.

### Step 3: Run the Main Program
Execute the main solution script:

```bash
python assignment1_solution.py
```

### Step 4: (Optional) Visualize Data
To see sample images from the dataset:

```bash
python visualize_data.py
```

---

## Output Files

After running `assignment1_solution.py`, the following files will be generated:

### 1. `gaussian_means.png`
**Answer to Question 2:** 
- Visualization of the 10 mean images (μₖ) for the Gaussian classifier
- Shows one 8×8 image for each digit (1, 2, 3, 4, 5, 6, 7, 8, 9, 0)

### 2. `naive_bayes_eta.png`
**Answer to Question 3:**
- Visualization of the 10 η parameter images for the Naive Bayes classifier
- Shows the probability p(bᵢ=1|Cₖ) for each feature and class
- Displayed as 8×8 images for each digit

### 3. `results.txt`
**Answers to Questions 2 and 4:**
- The shared variance σ² value for the Gaussian classifier
- Error table showing errors per digit for both classifiers
- Overall error rates for both classifiers

---

## Expected Output

When you run the main program, you will see:

1. **Console Output:**
   - Data loading confirmation
   - Training progress for both classifiers
   - Shared variance σ² value
   - Error counts per digit
   - Overall error rates

2. **Image Files:**
   - `gaussian_means.png`: Visual representation of learned means
   - `naive_bayes_eta.png`: Visual representation of learned probabilities

3. **Text File:**
   - `results.txt`: Complete numerical results for your report

---

## Assignment Questions Answered

### Question 2: Training Gaussian Classifiers
- **Plot:** See `gaussian_means.png`
- **σ² value:** See console output or `results.txt`

### Question 3: Training Naive Bayes Classifiers
- **Plot:** See `naive_bayes_eta.png`

### Question 4: Test Performance
- **Error table:** See console output or `results.txt`
- **Error rates:** See console output or `results.txt`

---

## Implementation Details

### Gaussian Classifier
- **Assumptions:**
  - Features are independent (zero covariance between features)
  - All features in all classes share the same variance σ²
  
- **Parameters estimated:**
  - μₖᵢ: Mean of feature i in class k (equation 3)
  - σ²: Shared variance (equation 4)
  - αₖ: Prior probability (uniform: 1/10)

- **Classification:** Uses Bayes rule to compute p(Cₖ|x) for each class

### Naive Bayes Classifier
- **Feature conversion:** Binary features (bᵢ = 1 if xᵢ > 0.5, else 0)
  
- **Parameters estimated:**
  - ηₖᵢ: Probability p(bᵢ=1|Cₖ) for each feature and class
  - αₖ: Prior probability (uniform: 1/10)

- **Classification:** Uses Bayes rule with binary feature probabilities

---

## Troubleshooting

### Error: "assignment1.mat not found"
- Ensure the data file is in the same directory as the Python scripts
- Check the filename is exactly `assignment1.mat`

### Error: "No module named 'scipy'"
- Install scipy: `pip install scipy`

### Error: "No module named 'matplotlib'"
- Install matplotlib: `pip install matplotlib`

### Images not displaying
- The images are saved as PNG files in the current directory
- Open them with any image viewer

---

## Code Structure

### Main Flow:
1. Load data from .mat file
2. Train Gaussian classifier → Calculate means and variance → Plot means
3. Train Naive Bayes classifier → Calculate η parameters → Plot η
4. Evaluate both classifiers on test data
5. Generate error tables and calculate error rates
6. Save all results

### Key Functions:
- `GaussianClassifier.train()`: Implements equations 3 and 4
- `GaussianClassifier.predict()`: Uses equation 1 for classification
- `NaiveBayesClassifier.train()`: Calculates η parameters
- `NaiveBayesClassifier.predict()`: Uses equations 5-7 for classification
- `evaluate_classifier()`: Tests classifier and computes errors

---

## Author Notes

This implementation:
- Uses vectorized operations for efficiency
- Avoids explicit loops where possible
- Includes numerical stability measures (epsilon for log calculations)
- Follows the assignment specifications exactly
- Generates all required outputs automatically

---

## Contact & Support

If you encounter any issues:
1. Check that all requirements are installed
2. Verify the data file is in the correct location
3. Ensure you're using Python 3.6 or higher

---

## License

This code is provided for educational purposes as part of Assignment 1.
